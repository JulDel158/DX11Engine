
#pragma pack_matrix(row_major)

cbuffer Object_cb : register(b0)
{
	matrix modeling;
};

cbuffer Frame_cb : register(b1)
{
	matrix view;
	matrix projection;
};

struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

VS_OUT main(VS_IN input) {
	
	VS_OUT output = (VS_OUT) 0;
	
	output.pos = mul(float4(input.pos, 1.0), modeling);
	output.pos = mul(output.pos, mul(view, projection));
	output.color = input.color;
	
	return output;
}
