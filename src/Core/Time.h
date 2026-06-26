#pragma once

namespace MiniEngine {

struct Time {
    static float deltaTime();
    static float timeSinceStart();
    static void  setDeltaTime(float dt);
    static void  setTimeSinceStart(float t);

private:
    static float s_deltaTime;
    static float s_timeSinceStart;
};

} // namespace MiniEngine
