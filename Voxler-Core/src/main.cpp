#include <Core/Application.hpp>

extern Voxler::Application* Voxler::EXT_InitApplication();

int main()
{
    // Could be pretty large, allocated on heap to save room on stack.
    // Lifetime is pretty simple. No need for smart pointers.
    Voxler::Application* app = Voxler::EXT_InitApplication();
    app->runApplication();
    delete app;
}
