module helloworld;

struct VertexInput {
    float3 position : POSITION;
    float2  uv:TEXCOORD0;
    float2 uv2: TEXCOORD1;
};



struct VertexOutput {
    float4 position : SV_Position;
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


ConstantBuffer<MultUniforms>  uniforms1: register(b1, space3);
// RWConstantBuffer<MultUniforms>  uniforms2: register(b2, space3);
// RConstantBuffer<MultUniforms>  uniforms3: register(b3, space3);

StructuredBuffer<MultUniforms>  uniforms5: register(b2, space1);
RWStructuredBuffer<MultUniforms>  uniforms0: register(b3, space1);

// RWStructuredBuffer<MultUniforms> ufs: register(b1, space2);


Texture2D tex: register(t1, space1);

[shader("vertex")]
VertexOutput mainVS(VertexInput input, VertexInput input2) {
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    return output;
}

[shader("fragment")]
float4 mainPS(VertexOutput input) : SV_Target {
    MultUniforms test;
    test.mult = 0.1;

    return float4(1.0 * test.mult, 0.0, 0.0, 1.0);
}
