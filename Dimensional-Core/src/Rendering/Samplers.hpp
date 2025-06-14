#ifndef DM_SAMPLERS_HPp
#define DM_SAMPLERS_HPp
#include <nvrhi/nvrhi.h>

namespace Dimensional {

inline nvrhi::SamplerHandle CreateLinearRepeatSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Repeat;
    desc.addressV = nvrhi::SamplerAddressMode::Repeat;
    desc.addressW = nvrhi::SamplerAddressMode::Repeat;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Point filtering with repeat addressing
inline nvrhi::SamplerHandle CreatePointRepeatSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Repeat;
    desc.addressV = nvrhi::SamplerAddressMode::Repeat;
    desc.addressW = nvrhi::SamplerAddressMode::Repeat;
    desc.minFilter = false;
    desc.magFilter = false;
    desc.mipFilter = false;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Linear filtering with clamp addressing
inline nvrhi::SamplerHandle CreateLinearClampSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Point filtering with clamp addressing
inline nvrhi::SamplerHandle CreatePointClampSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = false;
    desc.magFilter = false;
    desc.mipFilter = false;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

//=============================================================================
// ANISOTROPIC FILTERING SAMPLERS
//=============================================================================

// High-quality anisotropic sampler for textures
inline nvrhi::SamplerHandle CreateAnisotropicRepeatSampler(nvrhi::IDevice* device, float maxAnisotropy = 16.0f)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Repeat;
    desc.addressV = nvrhi::SamplerAddressMode::Repeat;
    desc.addressW = nvrhi::SamplerAddressMode::Repeat;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = maxAnisotropy;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Anisotropic sampler with clamp addressing
inline nvrhi::SamplerHandle CreateAnisotropicClampSampler(nvrhi::IDevice* device, float maxAnisotropy = 16.0f)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = maxAnisotropy;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

//=============================================================================
// BORDER SAMPLERS
//=============================================================================

// Black border sampler
inline nvrhi::SamplerHandle CreateLinearBorderSampler(nvrhi::IDevice* device, const nvrhi::Color& borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f))
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Border;
    desc.addressV = nvrhi::SamplerAddressMode::Border;
    desc.addressW = nvrhi::SamplerAddressMode::Border;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = borderColor;
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// White border sampler
inline nvrhi::SamplerHandle CreateWhiteBorderSampler(nvrhi::IDevice* device)
{
    return CreateLinearBorderSampler(device, nvrhi::Color(1.0f, 1.0f, 1.0f, 1.0f));
}

//=============================================================================
// MIRROR SAMPLERS
//=============================================================================

// Linear filtering with mirror addressing
inline nvrhi::SamplerHandle CreateLinearMirrorSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Mirror;
    desc.addressV = nvrhi::SamplerAddressMode::Mirror;
    desc.addressW = nvrhi::SamplerAddressMode::Mirror;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Point filtering with mirror addressing
inline nvrhi::SamplerHandle CreatePointMirrorSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Mirror;
    desc.addressV = nvrhi::SamplerAddressMode::Mirror;
    desc.addressW = nvrhi::SamplerAddressMode::Mirror;
    desc.minFilter = false;
    desc.magFilter = false;
    desc.mipFilter = false;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

//=============================================================================
// COMPARISON SAMPLERS (for shadow mapping)
//=============================================================================

// Linear comparison sampler for shadow maps
inline nvrhi::SamplerHandle CreateLinearComparisonSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(1.0f, 1.0f, 1.0f, 1.0f); // White border for shadows
    desc.reductionType = nvrhi::SamplerReductionType::Comparison;
    return device->createSampler(desc);
}

// Point comparison sampler for shadow maps
inline nvrhi::SamplerHandle CreatePointComparisonSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = false;
    desc.magFilter = false;
    desc.mipFilter = false;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(1.0f, 1.0f, 1.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Comparison;
    return device->createSampler(desc);
}

//=============================================================================
// REDUCTION SAMPLERS (for advanced sampling operations)
//=============================================================================

// Minimum reduction sampler
inline nvrhi::SamplerHandle CreateMinReductionSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(1.0f, 1.0f, 1.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Minimum;
    return device->createSampler(desc);
}

// Maximum reduction sampler
inline nvrhi::SamplerHandle CreateMaxReductionSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 0.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Maximum;
    return device->createSampler(desc);
}

//=============================================================================
// SPECIALIZED SAMPLERS
//=============================================================================

// Sampler for UI/HUD elements (point, clamp, no mips)
inline nvrhi::SamplerHandle CreateUISampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true; // Linear for smooth UI
    desc.magFilter = true;
    desc.mipFilter = false; // No mips for UI
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 0.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Sampler for skybox/cubemap (linear, clamp)
inline nvrhi::SamplerHandle CreateSkyboxSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Clamp;
    desc.addressV = nvrhi::SamplerAddressMode::Clamp;
    desc.addressW = nvrhi::SamplerAddressMode::Clamp;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Sampler for noise textures (point, repeat, no mips)
inline nvrhi::SamplerHandle CreateNoiseSampler(nvrhi::IDevice* device)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Repeat;
    desc.addressV = nvrhi::SamplerAddressMode::Repeat;
    desc.addressW = nvrhi::SamplerAddressMode::Repeat;
    desc.minFilter = false; // Point for crisp noise
    desc.magFilter = false;
    desc.mipFilter = false; // No mips for noise
    desc.maxAnisotropy = 1.0f;
    desc.mipBias = 0.0f;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

// Sampler with custom mip bias for sharp/soft appearance
inline nvrhi::SamplerHandle CreateBiasedSampler(nvrhi::IDevice* device, float mipBias)
{
    nvrhi::SamplerDesc desc;
    desc.addressU = nvrhi::SamplerAddressMode::Repeat;
    desc.addressV = nvrhi::SamplerAddressMode::Repeat;
    desc.addressW = nvrhi::SamplerAddressMode::Repeat;
    desc.minFilter = true;
    desc.magFilter = true;
    desc.mipFilter = true;
    desc.maxAnisotropy = 4.0f;
    desc.mipBias = mipBias;
    desc.borderColor = nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f);
    desc.reductionType = nvrhi::SamplerReductionType::Standard;
    return device->createSampler(desc);
}

//=============================================================================
// SAMPLER COLLECTION STRUCT
//=============================================================================

struct SamplerCollection {
    // Basic samplers
    std::pair<nvrhi::SamplerHandle, u32> linearRepeat;
    std::pair<nvrhi::SamplerHandle, u32> pointRepeat;
    std::pair<nvrhi::SamplerHandle, u32> linearClamp;
    std::pair<nvrhi::SamplerHandle, u32> pointClamp;

    // Anisotropic samplers
    std::pair<nvrhi::SamplerHandle, u32> anisotropicRepeat;
    std::pair<nvrhi::SamplerHandle, u32> anisotropicClamp;

    // Border samplers
    std::pair<nvrhi::SamplerHandle, u32> blackBorder;
    std::pair<nvrhi::SamplerHandle, u32> whiteBorder;
    // Mirror samplers
    std::pair<nvrhi::SamplerHandle, u32> linearMirror;
    std::pair<nvrhi::SamplerHandle, u32> pointMirror;
    // Comparison sample
    std::pair<nvrhi::SamplerHandle, u32> linearComparison;
    std::pair<nvrhi::SamplerHandle, u32> pointComparison;
    // Reduction sampler
    std::pair<nvrhi::SamplerHandle, u32> minReduction;
    std::pair<nvrhi::SamplerHandle, u32> maxReduction;

    // Specialized sampl
    std::pair<nvrhi::SamplerHandle, u32> ui;
    std::pair<nvrhi::SamplerHandle, u32> skybox;
    std::pair<nvrhi::SamplerHandle, u32> noise;
};

// Create all common samplers at once
inline SamplerCollection CreateAllSamplers(nvrhi::IDevice* device)
{
    SamplerCollection samplers;

    // Basic samplers
    samplers.linearRepeat = { CreateLinearRepeatSampler(device), 0 };
    samplers.pointRepeat = { CreatePointRepeatSampler(device), 1 };
    samplers.linearClamp = { CreateLinearClampSampler(device), 2 };
    samplers.pointClamp = { CreatePointClampSampler(device), 3 };

    // Anisotropic samplers
    samplers.anisotropicRepeat = { CreateAnisotropicRepeatSampler(device), 4 };
    samplers.anisotropicClamp = { CreateAnisotropicClampSampler(device), 5 };

    // Border samplers
    samplers.blackBorder = { CreateLinearBorderSampler(device), 6 };
    samplers.whiteBorder = { CreateWhiteBorderSampler(device), 7 };

    // Mirror samplers
    samplers.linearMirror = { CreateLinearMirrorSampler(device), 8 };
    samplers.pointMirror = { CreatePointMirrorSampler(device), 9 };

    // Comparison samplers
    samplers.linearComparison = { CreateLinearComparisonSampler(device), 10 };
    samplers.pointComparison = { CreatePointComparisonSampler(device), 11 };

    // Reduction samplers
    samplers.minReduction = { CreateMinReductionSampler(device), 12 };
    samplers.maxReduction = { CreateMaxReductionSampler(device), 13 };

    // Specialized samplers
    samplers.ui = { CreateUISampler(device), 14 };
    samplers.skybox = { CreateSkyboxSampler(device), 15 };
    samplers.noise = { CreateNoiseSampler(device), 16 };

    return samplers;
}

}
#endif // DM_SAMPLERS_HPP
