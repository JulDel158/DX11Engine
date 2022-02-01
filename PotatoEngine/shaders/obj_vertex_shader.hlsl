
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

//struct DirLight {
//    float4 color;
//    float3 direction;
//};

//struct PointLight {
//    float4 color;
//    float3 pos;
//    float radius;
//};

//struct ConeLight {
//    float4 color;
//    float3 pos;
//    float ratio;
//    float3 direction;
//};

cbuffer Scene_cb : register(b3) {
     // point light
    float4 p_color;
    float3 p_pos;
    float p_radius;
    // directional light
    float4 d_color;
    float3 d_direction;
    // cone light
    float4 c_color;
    float3 c_pos;
    float c_ratio;
    float3 c_direction;
    // ambient light
    float4 ambientTerm;
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
    float4 nrm : NORMAL;
    float4 worldPos : POSITION;
    float4 camPos : CAMPOS;
    float2 uv : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
	
    VS_OUT output = (VS_OUT) 0;
	
    output.pos = mul(float4(input.pos, 1.0f), modeling);
    output.pos = mul(output.pos, mul(view, projection));
    
    //for (int i = 0; i < 3; ++i) {
    //    output.camPos[i] = -dot(view[3].xyz, view[i].xyz);
    //}
    //output.camPos.w = 1.0f;
    
    output.nrm = mul(float4(input.nrm, 0.0f), modeling);
    output.uv = input.uv;
	
    return output;
}
