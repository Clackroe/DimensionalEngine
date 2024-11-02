#ifndef DM_CAMERA_H
#define DM_CAMERA_H

#include <core.hpp>
#include <glm/glm.hpp>
namespace Dimensional {
class DMCORE_API Camera {
public:
    Camera() = default;
    Camera(const glm::mat4& projectionMatrix)
        : m_Projection(projectionMatrix) {};

    virtual ~Camera() = default;
    inline glm::mat4 getProjection() { return m_Projection; }

    static std::vector<glm::vec4> calcFrustumCorners(glm::mat4 view, glm::mat4 proj) { return calcFrustumCorners(proj * view); };
    static std::vector<glm::vec4> calcFrustumCorners(glm::mat4 viewProj);

    static glm::mat4 calcTightOrthoProjection(glm::mat4 view, glm::vec3 lightDir, float near, float far, float fov, float aspect, float zMultiplier);

protected:
    glm::mat4 m_Projection = glm::mat4(1.0f);
};
}

#endif
