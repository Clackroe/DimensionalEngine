#ifndef DM_CAMERA_H
#define DM_CAMERA_H

#include <glm/glm.hpp>
namespace Dimensional {
class Camera {
public:
    Camera() = default;
    Camera(const glm::mat4& projectionMatrix)
        : m_Projection(projectionMatrix) {};

    virtual ~Camera() = default;
    inline glm::mat4 getProjection() { return m_Projection; }

protected:
    glm::mat4 m_Projection = glm::mat4(1.0f);
};
}

#endif
