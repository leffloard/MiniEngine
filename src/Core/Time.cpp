#include "Core/Time.h"

namespace MiniEngine {

float Time::s_deltaTime      = 0.0f;
float Time::s_timeSinceStart = 0.0f;

float Time::deltaTime()       { return s_deltaTime; }
float Time::timeSinceStart()  { return s_timeSinceStart; }
void  Time::setDeltaTime(float dt)       { s_deltaTime = dt; }
void  Time::setTimeSinceStart(float t)   { s_timeSinceStart = t; }

} // namespace MiniEngine
