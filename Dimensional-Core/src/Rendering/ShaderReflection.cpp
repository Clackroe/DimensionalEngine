#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderCompiler.hpp>
#include <Rendering/ShaderReflection.hpp>
#include <algorithm>
#include <slang.h>

namespace Dimensional {

ShaderReflection ShaderReflection::extractFromProgram(Slang::ComPtr<slang::IComponentType> linkedProgram, const EntryPointDescription& entryPointDesc)
{
    ShaderReflection reflection;

    auto programReflection = linkedProgram->getLayout();
    if (!programReflection) {
        DM_CORE_ERROR("Failed to get program reflection for entry point: {0}", entryPointDesc.name);
        return reflection;
    }

    auto entryPointReflection = programReflection->getEntryPointByIndex(0);
    if (!entryPointReflection) {
        DM_CORE_ERROR("Failed to get entry point reflection for: {0}", entryPointDesc.name);
        return reflection;
    }

    // Extract different types of reflection data
    if (entryPointDesc.type == nvrhi::ShaderType::Vertex) {
        reflection.extractInputLayout(entryPointReflection);
    }

    if (entryPointDesc.type == nvrhi::ShaderType::Compute) {
        reflection.extractWorkGroupSize(entryPointReflection);
    }

    reflection.extractResourceBindings(programReflection);
    reflection.extractConstantBuffers(programReflection);

    return reflection;
}

void ShaderReflection::extractInputLayout(slang::EntryPointReflection* entryPoint)
{
    SlangUInt paramCount = entryPoint->getParameterCount();
    uint32_t currentOffset = 0;

    for (SlangUInt i = 0; i < paramCount; ++i) {
        auto param = entryPoint->getParameterByIndex(i);
        if (param->getCategory() == slang::ParameterCategory::VaryingInput) {
            ShaderInputElement element;
            element.semanticName = param->getName();
            element.semanticIndex = 0; // TODO: Parse semantic index from name if needed
            element.format = slangTypeToNVRHIFormat(param->getType());
            element.inputSlot = 0; // Default to slot 0, can be overridden
            element.offset = currentOffset;
            element.isPerInstance = false; // TODO: Detect from semantic or attributes

            m_inputElements.push_back(element);

            // Calculate next offset based on format size
            currentOffset += GetFormatSize(element.format);
        }
    }
}

void ShaderReflection::extractResourceBindings(slang::ShaderReflection* program)
{
    SlangUInt paramCount = program->getParameterCount();

    for (SlangUInt i = 0; i < paramCount; ++i) {
        auto param = program->getParameterByIndex(i);
        auto category = param->getCategory();

        if (category == slang::ParameterCategory::ShaderResource || category == slang::ParameterCategory::UnorderedAccess || category == slang::ParameterCategory::SamplerState) {

            ShaderResourceBinding binding;
            binding.name = param->getName();
            binding.binding = static_cast<uint32_t>(param->getBindingIndex());
            binding.set = static_cast<uint32_t>(param->getBindingSpace());
            binding.type = slangResourceTypeToNVRHI(param->getType());

            // Check if it's an array
            auto type = param->getType();
            if (type->getKind() == slang::TypeReflection::Kind::Array) {
                binding.isArray = true;
                binding.arraySize = static_cast<uint32_t>(type->getElementCount());
            }

            m_resourceBindings.push_back(binding);
        }
    }
}

void ShaderReflection::extractConstantBuffers(slang::ShaderReflection* program)
{
    SlangUInt paramCount = program->getParameterCount();

    for (SlangUInt i = 0; i < paramCount; ++i) {
        auto param = program->getParameterByIndex(i);

        if (param->getCategory() == slang::ParameterCategory::ConstantBuffer) {
            ShaderConstantBuffer cb;
            cb.name = param->getName();
            cb.binding = static_cast<uint32_t>(param->getBindingIndex());
            cb.set = static_cast<uint32_t>(param->getBindingSpace());

            // Get the type layout instead of type reflection for size information
            auto paramLayout = param->getTypeLayout();
            if (paramLayout) {
                cb.size = static_cast<uint32_t>(paramLayout->getSize());

                // Extract CB members using type layout
                auto cbType = param->getType();
                SlangUInt fieldCount = cbType->getFieldCount();
                for (SlangUInt j = 0; j < fieldCount; ++j) {
                    auto field = cbType->getFieldByIndex(j);
                    auto fieldLayout = paramLayout->getFieldByIndex(j);

                    if (fieldLayout) {
                        uint32_t offset = static_cast<uint32_t>(fieldLayout->getOffset());
                        cb.members.emplace_back(field->getName(), offset);
                    } else {
                        // Fallback: if no layout info, assume sequential packing
                        // This is not ideal but prevents crashes
                        cb.members.emplace_back(field->getName(), 0);
                    }
                }
            } else {
                // Fallback for when type layout is not available
                auto cbType = param->getType();
                // Try to get size from type if available, otherwise use 0
                cb.size = 0; // We can't determine size without layout info

                SlangUInt fieldCount = cbType->getFieldCount();
                for (SlangUInt j = 0; j < fieldCount; ++j) {
                    auto field = cbType->getFieldByIndex(j);
                    cb.members.emplace_back(field->getName(), 0); // No offset info available
                }
            }

            m_constantBuffers.push_back(cb);
        }
    }
}

void ShaderReflection::extractWorkGroupSize(slang::EntryPointReflection* entryPoint)
{
    SlangUInt sizeAlongAxis[3];
    entryPoint->getComputeThreadGroupSize(3, sizeAlongAxis);
    m_workGroupSize.x = static_cast<uint32_t>(sizeAlongAxis[0]);
    m_workGroupSize.y = static_cast<uint32_t>(sizeAlongAxis[1]);
    m_workGroupSize.z = static_cast<uint32_t>(sizeAlongAxis[2]);
}

// Fix: Return InputLayoutDesc instead of IInputLayout (which is abstract)
// nvrhi::InputLayoutHandle ShaderReflection::createInputLayoutDesc() const
// {
//     nvrhi::InputLayoutDesc desc;
//
//     for (const auto& element : m_inputElements) {
//         nvrhi::VertexAttributeDesc attrib;
//         attrib.name = element.semanticName.c_str();
//         attrib.format = element.format;
//         attrib.offset = element.offset;
//         attrib.bufferIndex = element.inputSlot;
//         attrib.isInstanced = element.isPerInstance;
//
//         desc.addAttribute(attrib);
//     }
//
//     return desc;
// }

// Updated to handle binding sets properly
std::vector<nvrhi::BindingLayoutDesc> ShaderReflection::createBindingLayoutDescs() const
{
    // Group bindings by set/space
    std::map<uint32_t, std::vector<nvrhi::BindingLayoutItem>> bindingsBySet;

    // Process constant buffers
    for (const auto& cb : m_constantBuffers) {
        nvrhi::BindingLayoutItem item;
        item.type = nvrhi::ResourceType::ConstantBuffer;
        item.slot = cb.binding;
        bindingsBySet[cb.set].push_back(item);
    }

    // Process resource bindings
    for (const auto& binding : m_resourceBindings) {
        nvrhi::BindingLayoutItem item;
        item.type = binding.type;
        item.slot = binding.binding;
        if (binding.isArray) {
            item.size = binding.arraySize;
        }
        bindingsBySet[binding.set].push_back(item);
    }

    // Create binding layout descs for each set
    std::vector<nvrhi::BindingLayoutDesc> layoutDescs;

    // Find the maximum set index to ensure we create layouts for all sets
    uint32_t maxSet = 0;
    if (!bindingsBySet.empty()) {
        maxSet = bindingsBySet.rbegin()->first;
    }

    // Create layout descs for sets 0 through maxSet
    for (uint32_t setIndex = 0; setIndex <= maxSet; ++setIndex) {
        nvrhi::BindingLayoutDesc desc;

        if (bindingsBySet.find(setIndex) != bindingsBySet.end()) {
            auto& bindings = bindingsBySet[setIndex];

            // Sort by slot for consistent layout
            std::sort(bindings.begin(), bindings.end(),
                [](const nvrhi::BindingLayoutItem& a, const nvrhi::BindingLayoutItem& b) {
                    return a.slot < b.slot;
                });

            for (const auto& item : bindings) {
                desc.bindings.push_back(item);
            }
        }

        layoutDescs.push_back(desc);
    }

    return layoutDescs;
}

bool ShaderReflection::hasConstantBuffer(const std::string& name) const
{
    return std::any_of(m_constantBuffers.begin(), m_constantBuffers.end(),
        [&name](const ShaderConstantBuffer& cb) { return cb.name == name; });
}

bool ShaderReflection::hasResourceBinding(const std::string& name) const
{
    return std::any_of(m_resourceBindings.begin(), m_resourceBindings.end(),
        [&name](const ShaderResourceBinding& binding) { return binding.name == name; });
}

const ShaderConstantBuffer* ShaderReflection::getConstantBuffer(const std::string& name) const
{
    auto it = std::find_if(m_constantBuffers.begin(), m_constantBuffers.end(),
        [&name](const ShaderConstantBuffer& cb) { return cb.name == name; });
    return (it != m_constantBuffers.end()) ? &(*it) : nullptr;
}

const ShaderResourceBinding* ShaderReflection::getResourceBinding(const std::string& name) const
{
    auto it = std::find_if(m_resourceBindings.begin(), m_resourceBindings.end(),
        [&name](const ShaderResourceBinding& binding) { return binding.name == name; });
    return (it != m_resourceBindings.end()) ? &(*it) : nullptr;
}

void ShaderReflection::printReflectionInfo() const
{
    DM_CORE_INFO("=== Shader Reflection Info ===");

    if (!m_inputElements.empty()) {
        DM_CORE_INFO("Input Elements ({0}):", m_inputElements.size());
        for (const auto& element : m_inputElements) {
            DM_CORE_INFO("  - {0}: slot={1}, offset={2}, format={3}",
                element.semanticName, element.inputSlot, element.offset, static_cast<int>(element.format));
        }
    }

    if (!m_constantBuffers.empty()) {
        DM_CORE_INFO("Constant Buffers ({0}):", m_constantBuffers.size());
        for (const auto& cb : m_constantBuffers) {
            DM_CORE_INFO("  - {0}: binding={1}, set={2}, size={3} bytes",
                cb.name, cb.binding, cb.set, cb.size);
            for (const auto& member : cb.members) {
                DM_CORE_INFO("    * {0}: offset={1}", member.first, member.second);
            }
        }
    }

    if (!m_resourceBindings.empty()) {
        DM_CORE_INFO("Resource Bindings ({0}):", m_resourceBindings.size());
        for (const auto& binding : m_resourceBindings) {
            DM_CORE_INFO("  - {0}: binding={1}, set={2}, type={3}{4}",
                binding.name, binding.binding, binding.set, static_cast<int>(binding.type),
                binding.isArray ? std::string(" [") + std::to_string(binding.arraySize) + "]" : "");
        }
    }

    if (m_workGroupSize.x > 1 || m_workGroupSize.y > 1 || m_workGroupSize.z > 1) {
        DM_CORE_INFO("Work Group Size: {0}x{1}x{2}", m_workGroupSize.x, m_workGroupSize.y, m_workGroupSize.z);
    }
}

std::string ShaderReflection::getReflectionSummary() const
{
    std::ostringstream oss;
    oss << "Reflection Summary: ";
    oss << m_inputElements.size() << " inputs, ";
    oss << m_constantBuffers.size() << " CBs, ";
    oss << m_resourceBindings.size() << " resources";

    return oss.str();
}

}
