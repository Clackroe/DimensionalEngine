#include <Core/Application.hpp>

extern Dimensional::Application* Dimensional::EXT_InitApplication();

int main()
{
    // Could be pretty large, allocated on heap to save room on stack.
    // Lifetime is pretty simple. No need for smart pointers.
    Dimensional::Application* app = Dimensional::EXT_InitApplication();
    app->runApplication();
    delete app;
}
