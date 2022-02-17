struct particle
{
    float3 pos;
    float scale;
};

StructuredBuffer<particle> inputParticle : register(t0); // SRV

#pragma pack_matrix(row_major)

cbuffer Frame_cb : register(b1) // binded once every frame
{
    matrix view;
};

cbuffer Window_cb : register(b2) // binded whenever the window changes
{
    matrix projection;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXTCOORD;
};

struct empty{}; // this structs purpose is to allow this shader to compile as I need to provide

[maxvertexcount(4)]
void main(
point empty nothing[1],
uint index : SV_PrimitiveID,
inout TriangleStream<GS_OUT> output)
{
    GS_OUT quad[4];
    const float4 position = mul(float4(inputParticle[index].pos, 1), view);
    const float4 screenPos = mul(position, projection);
    
    if (abs(screenPos.x) <= screenPos.w && abs(screenPos.y) <= screenPos.w) // clipping particles: we may only want to do this occasionally, we should add some sort of check to turn this behavior on or off
    {
        const float scale = 0.5f * inputParticle[index].scale;
        
        // bottom left
        quad[0].pos = float4(position.x - scale, position.y - scale, position.zw);
        quad[0].uv = float2(0.f, 1.f);
    
        // top left
        quad[1].pos = float4(position.x - scale, position.y + scale, position.zw);
        quad[1].uv = float2(0.f, 0.f);
    
        // bottom right
        quad[2].pos = float4(position.x + scale, position.y - scale, position.zw);
        quad[2].uv = float2(1.f, 1.f);
    
        // top right
        quad[3].pos = float4(position.x + scale, position.y + scale, position.zw);
        quad[3].uv = float2(1.f, 0.f);
    
        for (int i = 0; i < 4; ++i)
        {
            quad[i].pos = mul(quad[i].pos, projection);
            output.Append(quad[i]);
        }
    }
}