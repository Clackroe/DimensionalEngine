#include "EngineAPI.hpp"
#include "ReflectionHelpers.hpp"
#include <NativeScripting.hpp>

using namespace ScriptingCore;
using namespace Dimensional;

class Player : public NativeScriptableEntity {
public:
    DM_GENERATED_BODY(Player)

    TransformCompHandle* transform = 0;
    DM_PROPERTY(Player, float, speed, 30);
    DM_PROPERTY(Player, float, testFloat, 10);

    DM_PROPERTY(Player, uint64_t, testVar, 30);

    DM_PROPERTY(Player, glm::vec3, testVec, glm::vec3(0, 22, 31));

    Player(uint64_t id)
        : NativeScriptableEntity(id)
    {
        transform = ScriptCoreLink::getComponentAPI()->Transform_GetComp(m_EntityHandle);
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

    void create() {

    };

    void destroy() {

    };
};

REGISTER_PROPERTY(Player, speed)
REGISTER_PROPERTY(Player, testFloat)
REGISTER_PROPERTY(Player, testVar)
REGISTER_PROPERTY(Player, testVec)

REGISTER_SCRIPT(Player)
