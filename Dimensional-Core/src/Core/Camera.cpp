
#include <Core/Camera.hpp>
#include <vector>
namespace Dimensional {

std::vector<glm::vec4> Camera::calcFrustumCorners(glm::mat4 viewProj)
{
    const glm::mat4 inverse = glm::inverse(viewProj);
    std::vector<glm::vec4> corners;
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 2; z++) {
                const glm::vec4 point = inverse * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                corners.push_back(point / point.w);
            }
        }
    }
    return corners;
}

glm::mat4 Camera::calcTightOrthoProjection(glm::mat4 view, glm::vec3 lightDir, float near, float far, float fov, float aspect, float zMultiplier)
{
    // Camera Projection inside cascade
    const glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, near, far);
    const std::vector<glm::vec4> corners = calcFrustumCorners(view, proj);

    glm::vec3 center = glm::vec3(0);
    for (const glm::vec4 c : corners) {
        center += glm::vec3(c);
    }
    center /= corners.size();

    const glm::mat4 lightView = glm::lookAt(center + lightDir, center, glm::vec3(0, 1, 0));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::lowest();
    for (const auto& v : corners) {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    if (minZ < 0) {
        minZ *= zMultiplier;
    } else {
        minZ /= zMultiplier;
    }
    if (maxZ < 0) {
        maxZ /= zMultiplier;
    } else {
        maxZ *= zMultiplier;
    }
    auto temp = -minZ;
    minZ = -maxZ;
    maxZ = temp;
    auto mid = (maxZ - minZ) / 2;
    minZ -= mid * 5.0f;
    maxZ += mid * 5.0f;

    const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
    return lightProjection * lightView;
}

}
