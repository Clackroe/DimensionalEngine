module helloworld;

#include <Dimensional.slang>

struct VertexInput {
    float3 position : POSITION;
    float2  uv:TEXCOORD0;
    float2 uv2: TEXCOORD1;
};



struct VertexOutput {
    float4 position : SV_Position;
    float2 uv;
};



struct Test {

    int t;
    float3 v;
}

struct MultUniforms {
    float mult;
    float3 m;
    float i;
    float4x4 b;
    Test[32] arr;
};



Texture2D tex: PIPELINE(t0);


[shader("vertex")]
VertexOutput mainVS(VertexInput input, VertexInput input2) {
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.uv = input.uv;
    return output;
}

[shader("fragment")]
float4 mainPS(VertexOutput input) : SV_Target {

    return float4(tex.Sample(linearRepeat, input.uv, 0).rgb, 1.0);
}
