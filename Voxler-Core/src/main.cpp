#include <Core/Application.hpp>

int main()
{
    Voxler::Scope<Voxler::Application> app = Voxler::CreateScope<Voxler::Application>();
    app->runApplication();
}
