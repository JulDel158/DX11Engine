// texture2D diffuse : register(t0); TODO: UNCOMMENT AND ADD DEBUG TEXTURE
SamplerState samLinear : register(s0);

cbuffer Object_cb : register(b0) // binder per object per frame
{
    matrix modeling;
};

cbuffer Frame_cb : register(b1) // binded once every frame
{
    matrix view;
};

cbuffer Window_cb : register(b2) // binded whenever the window changes
{
    matrix projection;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 nrm : NORMAL;
    float4 worldPos : POSITION;
    float4 camPos : CAMPOS;
    float2 uv : TEXCOORD;
};

float4 main(VS_OUT input) : SV_Target
{
    float4 finalColor = (float4)0;
    
    // finalColor = diffuse.Sample(samLinear, input.uv);
    
    // return finalColor;
    return float4(0.5, 0.5, 0.5, 1.f);

}