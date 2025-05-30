#ifndef DM_SHADER_REFLECTION
#define DM_SHADER_REFLECTION

#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderHelpersAndEnums.hpp>
#include <map>
#include <slang-com-ptr.h>
#include <slang.h>
#include <sstream>

namespace Dimensional {

struct EntryPointDescription;

// Reflection data structures
struct ShaderResourceBinding {
    std::string name;
    uint32_t binding;
    uint32_t set; // Descriptor set for Vulkan, register space for D3D
    nvrhi::ResourceType type;
    uint32_t arraySize = 1;
    bool isArray = false;
};

struct ShaderConstantBuffer {
    std::string name;
    uint32_t binding;
    uint32_t set;
    uint32_t size;
    std::vector<std::pair<std::string, uint32_t>> members; // member name -> offset
};

struct ShaderInputElement {
    std::string semanticName;
    uint32_t semanticIndex;
    nvrhi::Format format;
    uint32_t inputSlot;
    uint32_t offset;
    bool isPerInstance = false;
};

struct WorkGroupSize {
    uint32_t x = 1, y = 1, z = 1;
};

class ShaderReflection {
public:
    ShaderReflection() = default;
    ~ShaderReflection() = default;

    static ShaderReflection extractFromProgram(Slang::ComPtr<slang::IComponentType> program, const EntryPointDescription& entryPointDesc);

    // Getters
    const std::vector<ShaderInputElement>& getInputElements() const { return m_inputElements; }
    const std::vector<ShaderResourceBinding>& getResourceBindings() const { return m_resourceBindings; }
    const std::vector<ShaderConstantBuffer>& getConstantBuffers() const { return m_constantBuffers; }
    const WorkGroupSize& getWorkGroupSize() const { return m_workGroupSize; }

    // NVRHI layout creation
    // nvrhi::InputLayoutHandle createInputLayoutDesc() const;

    // New method that properly handles binding sets/spaces
    std::vector<nvrhi::BindingLayoutDesc> createBindingLayoutDescs() const;

    // Query methods
    bool hasConstantBuffer(const std::string& name) const;
    bool hasResourceBinding(const std::string& name) const;
    const ShaderConstantBuffer* getConstantBuffer(const std::string& name) const;
    const ShaderResourceBinding* getResourceBinding(const std::string& name) const;

    // Debug/info methods
    void printReflectionInfo() const;
    std::string getReflectionSummary() const;

private:
    std::vector<ShaderInputElement> m_inputElements;
    std::vector<ShaderResourceBinding> m_resourceBindings;
    std::vector<ShaderConstantBuffer> m_constantBuffers;
    WorkGroupSize m_workGroupSize;

    // Extraction methods
    void extractInputLayout(slang::EntryPointReflection* entryPoint);
    void extractResourceBindings(slang::ShaderReflection* program);
    void extractConstantBuffers(slang::ShaderReflection* program);
    void extractWorkGroupSize(slang::EntryPointReflection* entryPoint);
};

}

#endif // DM_SHADER_REFLECTION
