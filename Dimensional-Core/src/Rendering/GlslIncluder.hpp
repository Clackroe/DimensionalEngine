
#ifndef DIM_GLSL_INCLUDER
#define DIM_GLSL_INCLUDER

#include <filesystem>
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
namespace Dimensional {

class GLSLIncluder : public shaderc::CompileOptions::IncluderInterface {
public:
    explicit GLSLIncluder(const std::filesystem::path& baseDir)
        : m_BaseDirectory(baseDir)
    {
    }

    shaderc_include_result* GetInclude(const char* requested_source,
        shaderc_include_type type,
        const char* requesting_source,
        size_t include_depth) override
    {
        std::string resolved_path;
        if (type == shaderc_include_type_relative) {
            resolved_path = std::filesystem::path(requesting_source).parent_path()
                / requested_source;
        } else {
            resolved_path = m_BaseDirectory / requested_source;
        }

        std::ifstream file(resolved_path);
        if (!file.is_open()) {
            return MakeErrorResult("Failed to open include file: " + resolved_path);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string* content = new std::string(buffer.str());
        std::string* path_copy = new std::string(resolved_path);

        shaderc_include_result* result = new shaderc_include_result();
        result->source_name = path_copy->c_str();
        result->source_name_length = path_copy->size();
        result->content = content->c_str();
        result->content_length = content->size();
        result->user_data = static_cast<void*>(content); // we'll free this later
        m_AllocatedPaths[path_copy->c_str()] = path_copy;

        return result;
    }

    void ReleaseInclude(shaderc_include_result* include_result) override
    {
        delete static_cast<std::string*>(include_result->user_data); // content
        delete m_AllocatedPaths[include_result->source_name]; // path_copy
        m_AllocatedPaths.erase(include_result->source_name);
        delete include_result;
    }

private:
    shaderc_include_result* MakeErrorResult(const std::string& message)
    {
        std::string* error_msg = new std::string(message);
        shaderc_include_result* result = new shaderc_include_result();
        result->source_name = "";
        result->source_name_length = 0;
        result->content = error_msg->c_str();
        result->content_length = error_msg->size();
        result->user_data = static_cast<void*>(error_msg);
        return result;
    }

    std::filesystem::path m_BaseDirectory;
    std::unordered_map<std::string, std::string*> m_AllocatedPaths;
};
};
#endif
