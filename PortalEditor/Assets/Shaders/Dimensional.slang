module Dimensional;

#define CONSTANT(binding) register(binding, space0)
#define FRAME(binding) register(binding, space1)
#define PIPELINE(binding) register(binding, space2)
#define MATERIAL(binding) register(binding, space3)

// ===== CONSTANTS =======

// -- Samplers

    SamplerState linearRepeat : CONSTANT(s0);
    SamplerState pointRepeat : CONSTANT(s1);
    SamplerState linearClamp : CONSTANT(s2);
    SamplerState pointClamp : CONSTANT(s3);
    SamplerState anisotropicRepeat : CONSTANT(s4);
    SamplerState anisotropicClamp : CONSTANT(s5);
    SamplerState blackBorder : CONSTANT(s6);
    SamplerState whiteBorder : CONSTANT(s7);
    SamplerState linearMirror : CONSTANT(s8);
    SamplerState pointMirror : CONSTANT(s9);
    SamplerState linearComparison : CONSTANT(s10);
    SamplerState pointComparison : CONSTANT(s11);
    SamplerState minReduction : CONSTANT(s12);
    SamplerState maxReduction : CONSTANT(s13);
    SamplerState ui : CONSTANT(s14);
    SamplerState skybox : CONSTANT(s15);
    SamplerState noise : CONSTANT(s16);

// --


