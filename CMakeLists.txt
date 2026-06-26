cmake_minimum_required(VERSION 3.20)
project(MiniEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vendor: stb_image.h for texture loading (download if missing)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor")
set(STB_IMAGE_H "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image.h")
if(NOT EXISTS "${STB_IMAGE_H}")
  message(STATUS "Downloading stb_image.h into vendor/...")
  file(DOWNLOAD "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" "${STB_IMAGE_H}" TLS_VERIFY OFF STATUS STB_DL_STATUS)
  list(GET STB_DL_STATUS 0 STB_DL_CODE)
  if(NOT STB_DL_CODE EQUAL 0)
    message(WARNING "Could not download stb_image.h. Place it in vendor/ from https://github.com/nothings/stb")
  endif()
endif()

# Shaders: copy to build tree and to exe output so they're found at runtime
set(SHADERS_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders")
set(SHADERS_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")
file(MAKE_DIRECTORY "${SHADERS_BUILD_DIR}")
file(GLOB SHADER_SOURCES "${SHADERS_SRC_DIR}/*.vert" "${SHADERS_SRC_DIR}/*.frag")
foreach(f ${SHADER_SOURCES})
  get_filename_component(fname ${f} NAME)
  configure_file("${f}" "${SHADERS_BUILD_DIR}/${fname}" COPYONLY)
endforeach()

add_executable(MiniEngine
    src/main.cpp
    src/Core/Time.cpp
    src/Core/Input.cpp
    src/Core/Assets.cpp
    src/Scene/Camera.cpp
    src/Scene/Scene.cpp
    src/Gfx/Shader.cpp
    src/Gfx/Mesh.cpp
    src/Gfx/Texture.cpp
    src/Gfx/Skybox.cpp
    src/Gfx/Renderer.cpp
    src/Gfx/DeferredRenderer.cpp
)

target_include_directories(MiniEngine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/vendor)

# Enable STB_IMAGE_AVAILABLE only if vendor/stb_image.h exists and is the full header (>100KB).
# Otherwise texture file loading is disabled; createCheckerboard() still works.
if(EXISTS "${STB_IMAGE_H}")
  file(SIZE "${STB_IMAGE_H}" STB_IMAGE_H_SIZE)
  if(STB_IMAGE_H_SIZE GREATER 100000)
    target_compile_definitions(MiniEngine PRIVATE STB_IMAGE_AVAILABLE=1)
  endif()
endif()

# SHADER_DIR: relative path "Shaders" (shaders copied to exe dir in POST_BUILD; run with cwd = exe dir)
target_compile_definitions(MiniEngine PRIVATE
    SHADER_DIR="Shaders"
)
# Copy shaders to executable directory so they're found when run from build or VS
add_custom_command(TARGET MiniEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SHADERS_BUILD_DIR}" "$<TARGET_FILE_DIR:MiniEngine>/Shaders"
    COMMENT "Copying shaders to executable directory"
)

# GLFW + GLAD + GLM via FetchContent (unchanged)
include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

target_link_libraries(MiniEngine PRIVATE glm glfw glad)

if(WIN32)
  target_link_libraries(MiniEngine PRIVATE opengl32)
endif()

if(MSVC)
  target_compile_options(MiniEngine PRIVATE /W4)
else()
  target_compile_options(MiniEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
