#include "EngineAPI.hpp"
#include "ReflectionHelpers.hpp"
#include <NativeScripting.hpp>
#include <string>

using namespace Dimensional;

class Player : public NativeScriptableEntity {
public:
    DM_GENERATED_BODY(Player)

    TransformCompHandle* transform = 0;
    DM_PROPERTY(Player, float, speed, 100);

    float testThing = 2.0;

    Player(u64 id)
        : NativeScriptableEntity(id)
    {
        transform = ScriptCoreLink::getComponentAPI()->Transform_GetComp(m_EntityHandle);

        Log::Info(("Offset of id: " + std::to_string(offsetof(Player, m_EntityHandle))).c_str());

        Log::Info(("Offset of trans: " + std::to_string(offsetof(Player, transform))).c_str());
        Log::Info(("Offset of speed: " + std::to_string(offsetof(Player, speed))).c_str());
    }

    void update()
    {

        glm::vec3 pos = ScriptCoreLink::getComponentAPI()->Transform_GetPosition(transform);

        if (Input::isKeyDown(Key::Up)) {
            Log::Info(("Test: " + std::to_string(testThing)).c_str());

            pos.z += 0.01 * speed;
        }
        if (Input::isKeyDown(Key::Down)) {
            pos.z -= 0.01 * speed;
        }
        if (Input::isKeyDown(Key::Left)) {
            pos.x -= 0.01 * speed;
        }
        if (Input::isKeyDown(Key::Right)) {
            pos.x += 0.01 * speed;
        }
        if (Input::isKeyDown(Key::P)) {
            pos.y += 0.01 * speed;
        }
        if (Input::isKeyDown(Key::L)) {
            pos.y -= 0.01 * speed;
        }

        ScriptCoreLink::getComponentAPI()->Transform_SetPosition(transform, pos);
    };

    void create()
    {
        Log::Warn("Creating...");
    };

    void destroy()
    {
        Log::Warn("Destroying...");
    };
};

// MemberRegistrar<float, Player::_do_not_use_speed> memReg_speed(Player::register_speed, Player::s_RegisterMembersFuncs);

REGISTER_SCRIPT(Player)