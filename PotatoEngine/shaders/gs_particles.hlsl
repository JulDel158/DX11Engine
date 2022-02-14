
#pragma pack_matrix(row_major)

cbuffer Window_cb : register(b2) // binded whenever the window changes
{
    matrix projection;
};


struct GS_IN
{
    float4 pos : SV_Position;
    float scale : SCALE;
};

struct GS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXTCOORD;
};

[maxvertexcount(4)]
void main(
point GS_IN input[1] : SV_Position,
inout TriangleStream<GS_OUT> output )
{
    GS_OUT quad[4];
    // top left
    quad[0].pos = float4(input[0].pos.x - 0.5f, input[0].pos.y + 0.5f, input[0].pos.zw) * input[0].scale;
    quad[0].uv = float2(0.f, 0.f);
    
    // bottom left
    quad[1] = float4(input[0].pos.x - 0.5f, input[0].pos.y - 0.5f, input[0].pos.zw) * input[0].scale;
    quad[1].uv = float2(0.f, 1.f);
    
    // bottom right
    quad[2] = float4(input[0].pos.x + 0.5f, input[0].pos.y - 0.5f, input[0].pos.zw) * input[0].scale;
    quad[2].uv = float2(1.f, 1.f);
    
    // top right
    quad[3] = float4(input[0].pos.x + 0.5f, input[0].pos.y + 0.5f, input[0].pos.zw) * input[0].scale;
    quad[3].uv = float2(1.f, 0.f);
    
    for (int i = 0; i < 4; ++i) {
        quad[i].pos = mul(quad[i].pos, projection);
        output.Append(quad[i]);
    }

}