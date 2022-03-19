
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

// THIS DATA IS BEING BINDED TO THE PIXEL SHADER, NOT THE VERTEX SHADER
//struct PointLight
//{
//    float4 color;
//    float3 pos;
//    float radius;
//};

//struct DirectionalLight
//{
//    float4 color;
//    float3 direction;
//};

//cbuffer Scene_cb : register(b3)
//{
//    // point light
//    PointLight plight;
//    // directional light
//    DirectionalLight dlight;
//    // cone light
//    //float4 c_color;
//    //float3 c_pos;
//    //float c_ratio;
//    //float3 c_direction;
//    // ambient light
//    float4 ambientLightColor;
//};

struct VS_IN
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 camPos : CAMPOS;
    float3 nrm : NORMAL;
    float u : TEXCOORD0;
    float3 worldPos : POSITION;
    float v : TEXCOORD1;
};

VS_OUT main(VS_IN input)
{
	
    VS_OUT output = (VS_OUT) 0;
	
    output.pos = mul(float4(input.pos, 1.0f), modeling);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, mul(view, projection));
    
    //for (int i = 0; i < 3; ++i) {
    //    output.camPos[i] = -dot(view[3].xyz, view[i].xyz);
    //}
    //output.camPos.w = 1.0f;
    output.nrm = mul(input.nrm, (float3x3)modeling);
    output.u = input.uv.x;
    output.v = input.uv.y;
	
    return output;
}
