#pragma pack_matrix(row_major)

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

struct VS_IN
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD1;
};

VS_OUT main(VS_IN input)
{
	
    VS_OUT output = (VS_OUT) 0;
	
    output.pos = mul(float4(input.pos, 1.0f), modeling);
    output.pos = mul(output.pos, mul(view, projection));
    
    output.localPos = input.pos;
	
    return output;
}