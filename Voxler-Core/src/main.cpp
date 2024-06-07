#include <Core/Application.hpp>

extern Voxler::Application* Voxler::EXT_InitApplication();

int main()
{
    Voxler::Application* app = Voxler::EXT_InitApplication();
    app->runApplication();
    delete app;
}
