#include "Log/log.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderCompiler.hpp>
#include <Rendering/ShaderReflection.hpp>
#include <algorithm>
#include <slang/slang.h>

namespace Dimensional {

ShaderReflection ShaderReflector::extractFromProgram(Slang::ComPtr<slang::IComponentType> linkedProgram, const EntryPointDescription& entryPointDesc)
{
    slang::ProgramLayout* layout = linkedProgram->getLayout();

    int parameters = layout->getParameterCount();

    for (int i = 0; i < parameters; i++) {

        auto param = layout->getParameterByIndex(i);
        DM_CORE_INFO("NAME: {}", param->getName());
        DM_CORE_INFO("\t BINDING: {}", param->getBindingIndex());
        DM_CORE_INFO("\t SPACE/SET: {}", param->getBindingSpace());
        param->getCategory();
    }

    return {};
}

}
