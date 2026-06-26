#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Core/Time.h"
#include "Core/Input.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Gfx/Shader.h"
#include "Gfx/Mesh.h"
#include "Gfx/Texture.h"
#include "Gfx/Skybox.h"
#include "Gfx/Lighting.h"
#include "Gfx/Renderer.h"
#include "Gfx/DeferredRenderer.h"

#include <iostream>
#include <string>
#include <cmath>

#ifndef SHADER_DIR
#define SHADER_DIR "Shaders"
#endif

static void framebuffer_size_callback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

static float s_lastMouseX = 640.0f;
static float s_lastMouseY = 360.0f;
static bool  s_firstMouse = true;

static void cursor_callback(GLFWwindow*, double xpos, double ypos) {
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(ypos);
    if (s_firstMouse) {
        s_lastMouseX = x;
        s_lastMouseY = y;
        s_firstMouse = false;
        return;
    }
    float dx = x - s_lastMouseX;
    float dy = s_lastMouseY - y;
    s_lastMouseX = x;
    s_lastMouseY = y;
    MiniEngine::Input::setMouseDelta(dx, dy);
}

static void scroll_callback(GLFWwindow*, double, double yoffset) {
    MiniEngine::Input::addScrollDelta(static_cast<float>(yoffset));
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "MiniEngine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window create failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD load failed\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    MiniEngine::Input::setWindow(window);

    MiniEngine::Shader shader;
    std::string vsPath = std::string(SHADER_DIR) + "/lit.vert";
    std::string fsPath = std::string(SHADER_DIR) + "/lit.frag";
    if (!shader.loadFromFile(vsPath, fsPath)) {
        vsPath = "src/Shaders/lit.vert";
        fsPath = "src/Shaders/lit.frag";
        if (!shader.loadFromFile(vsPath, fsPath)) {
            glfwDestroyWindow(window);
            glfwTerminate();
            return 1;
        }
    }

    MiniEngine::Shader skyboxShader;
    std::string skyVs = std::string(SHADER_DIR) + "/skybox.vert";
    std::string skyFs = std::string(SHADER_DIR) + "/skybox.frag";
    if (!skyboxShader.loadFromFile(skyVs, skyFs)) {
        skyVs = "src/Shaders/skybox.vert";
        skyFs = "src/Shaders/skybox.frag";
        skyboxShader.loadFromFile(skyVs, skyFs);
    }

    MiniEngine::Shader instancedShader;
    std::string iVs = std::string(SHADER_DIR) + "/lit_instanced.vert";
    std::string iFs = std::string(SHADER_DIR) + "/lit.frag";
    if (!instancedShader.loadFromFile(iVs, iFs)) {
        iVs = "src/Shaders/lit_instanced.vert";
        instancedShader.loadFromFile(iVs, iFs);
    }

    MiniEngine::Mesh cubeMesh   = MiniEngine::Mesh::createCube();
    MiniEngine::Mesh groundMesh = MiniEngine::Mesh::createGroundPlane(30.0f);

    MiniEngine::Texture2D checkerTex = MiniEngine::Texture2D::createCheckerboard(8, 256);

    MiniEngine::Scene scene = MiniEngine::Scene::createDemoScene(&cubeMesh, &groundMesh, &checkerTex, nullptr);
    size_t instancedStartIndex = scene.entities().size();
    scene.addCubeGrid(&cubeMesh, 500, 2.0f, nullptr);
    size_t instancedCount = scene.entities().size() - instancedStartIndex;
    bool instancedCubesVisible = true;

    MiniEngine::Skybox skybox;
    float skyTop[]    = { 0.25f, 0.45f, 0.7f };
    float skyBottom[] = { 0.6f, 0.6f, 0.65f };
    skybox.createProcedural(skyTop, skyBottom);

    MiniEngine::Shader gbufferShader;
    std::string gbVs = std::string(SHADER_DIR) + "/gbuffer.vert";
    std::string gbFs = std::string(SHADER_DIR) + "/gbuffer.frag";
    if (!gbufferShader.loadFromFile(gbVs, gbFs)) {
        gbVs = "src/Shaders/gbuffer.vert";
        gbFs = "src/Shaders/gbuffer.frag";
        gbufferShader.loadFromFile(gbVs, gbFs);
    }
    MiniEngine::Shader deferredLightShader;
    std::string dlVs = std::string(SHADER_DIR) + "/deferred_light.vert";
    std::string dlFs = std::string(SHADER_DIR) + "/deferred_light.frag";
    if (!deferredLightShader.loadFromFile(dlVs, dlFs)) {
        dlVs = "src/Shaders/deferred_light.vert";
        dlFs = "src/Shaders/deferred_light.frag";
        deferredLightShader.loadFromFile(dlVs, dlFs);
    }

    MiniEngine::Renderer renderer;
    renderer.setShader(&shader);
    renderer.setInstancedShader(&instancedShader);
    renderer.setSkyboxShader(&skyboxShader);

    MiniEngine::DeferredRenderer deferredRenderer;
    bool deferredEnabled = false;
    int winW = 1280, winH = 720;
    glfwGetFramebufferSize(window, &winW, &winH);
    if (deferredRenderer.init(winW, winH, SHADER_DIR)) {
        deferredRenderer.setGbufferShader(&gbufferShader);
        deferredRenderer.setLightingShader(&deferredLightShader);
    } else {
        std::cerr << "DeferredRenderer init failed; F9 deferred will be disabled.\n";
    }

    MiniEngine::Camera camera;
    camera.setMoveSpeed(3.2f);
    camera.setMouseSensitivity(0.07f);

    bool vsyncEnabled = true;
    bool showFps      = true;
    double lastFpsTime = glfwGetTime();
    int frames         = 0;

    while (!glfwWindowShouldClose(window)) {
        float current = static_cast<float>(glfwGetTime());
        MiniEngine::Time::setDeltaTime(current - MiniEngine::Time::timeSinceStart());
        MiniEngine::Time::setTimeSinceStart(current);

        MiniEngine::Input::updateFromWindow();

        if (MiniEngine::Input::keyDown(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, 1);

        if (MiniEngine::Input::keyPressed(GLFW_KEY_F1)) {
            vsyncEnabled = !vsyncEnabled;
            glfwSwapInterval(vsyncEnabled ? 1 : 0);
        }
        if (MiniEngine::Input::keyPressed(GLFW_KEY_F2))
            renderer.setGridEnabled(!renderer.gridEnabled());
        if (MiniEngine::Input::keyPressed(GLFW_KEY_F3))
            renderer.setWireframe(!renderer.wireframe());
        if (MiniEngine::Input::keyPressed(GLFW_KEY_F4)) {
            static bool cursorCaptured = true;
            cursorCaptured = !cursorCaptured;
            glfwSetInputMode(window, GLFW_CURSOR, cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
        if (MiniEngine::Input::keyPressed(GLFW_KEY_F5)) {
            instancedCubesVisible = !instancedCubesVisible;
            for (size_t i = instancedStartIndex; i < instancedStartIndex + instancedCount; ++i)
                scene.entities()[i].visible = instancedCubesVisible;
        }
        if (MiniEngine::Input::keyPressed(GLFW_KEY_F9)) {
            deferredEnabled = !deferredEnabled;
            if (deferredEnabled && deferredRenderer.isValid())
                std::cout << "Deferred rendering ON\n";
            else if (!deferredEnabled)
                std::cout << "Deferred rendering OFF (forward)\n";
        }

        float dx = 0.0f, dy = 0.0f;
        MiniEngine::Input::getMouseDelta(dx, dy);
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            camera.addYaw(dx * camera.mouseSensitivity());
            camera.addPitch(dy * camera.mouseSensitivity());
        }

        float scroll = MiniEngine::Input::scrollDelta();
        if (scroll != 0.0f) {
            if (MiniEngine::Input::keyDown(GLFW_KEY_LEFT_SHIFT))
                camera.setMoveSpeed(std::max(0.5f, camera.moveSpeed() + scroll * 0.5f));
            else
                camera.addFov(scroll * 2.0f);
        }

        float baseSpeed = camera.moveSpeed();
        if (MiniEngine::Input::keyDown(GLFW_KEY_LEFT_SHIFT)) baseSpeed *= 2.2f;
        float velocity = baseSpeed * MiniEngine::Time::deltaTime();

        if (MiniEngine::Input::keyDown(GLFW_KEY_W)) camera.setPosition(camera.position() + camera.front() * velocity);
        if (MiniEngine::Input::keyDown(GLFW_KEY_S)) camera.setPosition(camera.position() - camera.front() * velocity);
        if (MiniEngine::Input::keyDown(GLFW_KEY_A)) camera.setPosition(camera.position() - camera.right() * velocity);
        if (MiniEngine::Input::keyDown(GLFW_KEY_D)) camera.setPosition(camera.position() + camera.right() * velocity);
        if (MiniEngine::Input::keyDown(GLFW_KEY_SPACE)) camera.setPosition(camera.position() + camera.up() * velocity);
        if (MiniEngine::Input::keyDown(GLFW_KEY_LEFT_CONTROL)) camera.setPosition(camera.position() - camera.up() * velocity);

        int w = 0, h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        if (deferredEnabled && deferredRenderer.isValid() && (w != deferredRenderer.width() || h != deferredRenderer.height()))
            deferredRenderer.resize(w, h);
        float aspect = (h == 0) ? 1.0f : static_cast<float>(w) / static_cast<float>(h);

        MiniEngine::SceneLighting lighting;
        lighting.pointLightPos = glm::vec3(
            std::cos(current * 0.6f) * 6.0f,
            4.0f,
            std::sin(current * 0.6f) * 6.0f
        );
        lighting.specularIntensity = 0.5f;

        if (deferredEnabled && deferredRenderer.isValid()) {
            deferredRenderer.setGridEnabled(renderer.gridEnabled());
            glClearColor(0.06f, 0.08f, 0.16f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            deferredRenderer.geometryPass(scene, camera, aspect);
            deferredRenderer.lightingPass(lighting, camera, aspect);
            glViewport(0, 0, w, h);
            renderer.drawSkybox(&skybox, camera, aspect);
        } else {
            renderer.beginFrame(glm::vec3(0.06f, 0.08f, 0.16f));
            renderer.drawScene(scene, camera, lighting, aspect);
            renderer.drawSkybox(&skybox, camera, aspect);
        }

        if (showFps) {
            frames++;
            double now = glfwGetTime();
            if (now - lastFpsTime >= 1.0) {
                std::cout << "FPS: " << (frames / (now - lastFpsTime)) << "\n";
                frames = 0;
                lastFpsTime = now;
            }
        }

        MiniEngine::Input::endFrame();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
