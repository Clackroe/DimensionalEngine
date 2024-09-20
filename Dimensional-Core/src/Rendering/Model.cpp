// #include <assimp/material.h>
// #include "imgui.h"
#include <Rendering/Mesh.hpp>

#include <Assets/AssetManager.hpp>
#include <Rendering/Model.hpp>

#include <Rendering/Texture.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

namespace Dimensional {

Model::Model(ModelLoadSettings settings)
{
    m_Settings = settings;
    load();
}

void Model::load()
{
}

}
