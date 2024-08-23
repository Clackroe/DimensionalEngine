#include "GLFW/glfw3.h"
#include <Core/Time.hpp>

namespace Dimensional {

float Time::m_DeltaTime = 0.0f;
float Time::m_LastFrame = 0.0f;

void Time::calculateDelta()
{
    float current = glfwGetTime();
    m_DeltaTime = current - m_LastFrame;
    m_LastFrame = current;
}
}
