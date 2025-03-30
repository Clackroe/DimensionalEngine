#include "EngineAPI.hpp"
#include <NativeScriptableEntity.hpp>
#include <NativeScripting.hpp>

using namespace ScriptingCore;
using namespace Dimensional;

// DM-CLASS
class Player : public ScriptingCore::NativeScriptableEntity {
public:
    // DM-PROPERTY
    float speed = 2;

    // DM-PROPERTY
    float rotSpeed = 20;

    // DM-PROPERTY
    float testFloat = 20;

    // DM-PROPERTY
    uint64_t testVar = 30;

    // DM-PROPERTY
    glm::vec3 testVec = glm::vec3(0, 22, 31);

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
