module helloworld;

struct VertexInput {
    float3 position : POSITION;
};



struct VertexOutput {
    float4 position : SV_Position;
};


struct MultUniforms {
    int mult;
};


ConstantBuffer<MultUniforms>  uniforms: register(b2, space3);

[shader("vertex")]
VertexOutput mainVS(VertexInput input) {
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    return output;
}

[shader("fragment")]
float4 mainPS(VertexOutput input) : SV_Target {
    MultUniforms test;
    test.mult = 1;

    return float4(1.0 * test.mult, 0.0, 0.0, 1.0);
}
