#ifndef VAO_MANAGER_HPP
#define VAO_MANAGER_HPP

#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
#include "Rendering/VAO.hpp"
#include "core.hpp"
#include <Core/UUID.hpp>

namespace Dimensional {

union GraphicsVAO {
    OpenGLVAO glVAO = {};
};

struct VAOManager {

    static UUID CreateVAO();

    static void Bind(UUID id);
    static void UnBind(UUID id); // Maybe unecessary

    static void SetData(UUID id, const VAOData& data);

    static void DeleteVAO(UUID id);

private:
    static UMap<UUID, GraphicsVAO> s_VAOMap;
};

}

#endif // VAO_MANAGER_HPP
