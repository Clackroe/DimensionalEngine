#include "EngineAPI.hpp"
#include "ReflectionHelpers.hpp"
#include <NativeScripting.hpp>

#include <string>

using namespace ScriptingCore;
using namespace Dimensional;

class Player : public NativeScriptableEntity {
public:
    DM_GENERATED_BODY(Player)

    TransformCompHandle* transform = 0;
    DM_PROPERTY(Player, float, speed, 30);

    Player(uint64_t id)
        : NativeScriptableEntity(id)
    {
        transform = ScriptCoreLink::getComponentAPI()->Transform_GetComp(m_EntityHandle);

        Log::Info(("Hello from the constructor: " + std::to_string(m_EntityHandle)).c_str());
        //
        // Log::Info(("Offset of trans: " + std::to_string(offsetof(Player, transform))).c_str());
        // Log::Info(("Offset of speed: " + std::to_string(offsetof(Player, speed))).c_str());
    }

    void update()
    {

        glm::vec3 pos = ScriptCoreLink::getComponentAPI()->Transform_GetPosition(transform);

        if (Input::isKeyDown(Key::Up)) {
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
        Log::Info("Hello from the Create");

        // Log::Warn("Creating...");
    };

    void destroy()
    {
        Log::Info("Hello from the onDestroy");

        // Log::Warn("Destroying...");
    };
};

REGISTER_PROPERTY(Player, speed)

REGISTER_SCRIPT(Player)
