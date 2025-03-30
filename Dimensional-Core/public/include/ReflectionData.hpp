#ifndef REFLECTIONDATA_HPP
#define REFLECTIONDATA_HPP

#include <functional>
#include <map>
#include <string>
#include <unordered_map>

enum class ScriptMemberType {
    FLOAT,
    INT,
    U32,
    U64,
    GLM_VEC3,
    NONE
};

namespace Dimensional {

static std::map<std::string, ScriptMemberType> g_StringToScriptMember = {
    { "float", ScriptMemberType::FLOAT },
    { "int", ScriptMemberType::INT },
    { "uint32_t", ScriptMemberType::U32 },
    { "u32", ScriptMemberType::U32 },
    { "uint64_t", ScriptMemberType::U64 },
    { "u64", ScriptMemberType::U64 },
    { "glm::vec3", ScriptMemberType::GLM_VEC3 },
    { "vec3", ScriptMemberType::GLM_VEC3 }, // Just in case they use namespace glm
};

static std::map<std::string, std::string> g_StringToScriptMemberAsString = {
    { "float", "ScriptMemberType::FLOAT" },
    { "int", "ScriptMemberType::INT" },
    { "uint32_t", "ScriptMemberType::U32" },
    { "u32", "ScriptMemberType::U32" },
    { "uint64_t", "ScriptMemberType::U64" },
    { "u64", "ScriptMemberType::U64" },
    { "glm::vec3", "ScriptMemberType::GLM_VEC3" },
    { "vec3", "ScriptMemberType::GLM_VEC3" }, // Just in case they use namespace glm
};

static std::map<ScriptMemberType, std::string> g_ScriptMemberToString = {
    { ScriptMemberType::FLOAT, "float" },
    { ScriptMemberType::INT, "int" },
    { ScriptMemberType::U32, "u32" },
    { ScriptMemberType::U64, "u64" },
    { ScriptMemberType::GLM_VEC3, "glm::vec3" },
};

static std::map<ScriptMemberType, size_t> g_ScriptMemberToSize = {
    { ScriptMemberType::FLOAT, sizeof(float) },
    { ScriptMemberType::INT, sizeof(int) },
    { ScriptMemberType::U32, sizeof(unsigned int) },
    { ScriptMemberType::U64, sizeof(unsigned long) },
    { ScriptMemberType::GLM_VEC3, sizeof(float) * 3 }, // Don't want to include glm in this header
};

static std::map<ScriptMemberType, std::string> g_ScriptMemberToMemberUnionName = {
    { ScriptMemberType::FLOAT, "floatValue" },
    { ScriptMemberType::INT, "intValue" },
    { ScriptMemberType::U32, "u32Value" },
    { ScriptMemberType::U64, "u64Value" },
    { ScriptMemberType::GLM_VEC3, "vec3Value" },
};

}

#endif
