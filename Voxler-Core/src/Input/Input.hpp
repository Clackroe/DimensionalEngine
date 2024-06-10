#ifndef VX_INPUTH
#define VX_INPUTH
namespace Voxler {
class Input {
public:
    Input() = default;
    ~Input() = default;

private:
    static Input* s_Input;
};
}

#endif
