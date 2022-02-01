textureCUBE cubemap : register(t0);
SamplerState samLinear : register(s0);

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD1;
};

float4 main(VS_OUT input) : SV_Target
{
    return cubemap.Sample(samLinear, input.localPos);
}