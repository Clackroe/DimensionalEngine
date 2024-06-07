#include <voxler.hpp>

namespace Voxler {

class TestBed : public Application {
public:
    TestBed()
    {
    }

    ~TestBed()
    {
    }
};

Application* EXT_InitApplication()
{
    return new TestBed();
}
}
