Texture2D diffuse : register(t0);
SamplerState samLinear : register(s0);

struct GS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXTCOORD;
};

float4 main(GS_OUT input) : SV_Target
{
    float4 output = (float4) 0;
    
    output = diffuse.Sample(samLinear, input.uv);
    
    return saturate(output);
    
    // return float4(0.f, 0.f, 1.f, 1.f);
}