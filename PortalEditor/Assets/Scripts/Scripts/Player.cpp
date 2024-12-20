#include "EngineAPI.hpp"
#include "ReflectionHelpers.hpp"
#include <NativeScriptableEntity.hpp>
#include <NativeScripting.hpp>

using namespace ScriptingCore;
using namespace Dimensional;

class Player : public ScriptingCore::NativeScriptableEntity {
public:
    DM_GENERATED_BODY(Player)

    DM_PROPERTY(Player, float, speed, 30);

    DM_PROPERTY(Player, float, rotSpeed, 40);

    DM_PROPERTY(Player, float, testFloat, 20);

    DM_PROPERTY(Player, uint64_t, testVar, 30);

    DM_PROPERTY(Player, glm::vec3, testVec, glm::vec3(0, 22, 31));

    Player(uint64_t id)
        : NativeScriptableEntity(id)
    {
    }

    void update()
    {

        glm::vec3 pos = transform.getPosition();
        glm::vec3 rot = transform.getRotationDegrees();

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
        if (Input::isKeyDown(Key::N1)) {
            rot.y -= 0.01 * rotSpeed * 100;
        }
        if (Input::isKeyDown(Key::N3)) {
            rot.y += 0.01 * rotSpeed * 100;
        }

        transform.setPosition(pos);
        transform.setRotationDegrees(rot);
    };

    void create() {

    };

    void destroy() {

    };
};

REGISTER_PROPERTY(Player, speed)
REGISTER_PROPERTY(Player, rotSpeed)
REGISTER_PROPERTY(Player, testFloat)
REGISTER_PROPERTY(Player, testVar)
REGISTER_PROPERTY(Player, testVec)

REGISTER_SCRIPT(Player)
