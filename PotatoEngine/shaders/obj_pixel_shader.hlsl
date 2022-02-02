Texture2D diffuse : register(t0);
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

cbuffer Scene_cb : register(b3)
{
    // point light
    //float4 p_color;
    //float3 p_pos;
    //float p_radius;
    // directional light
    float4 d_color;
    float4 ambientLightColor;
    float3 d_direction;
    // cone light
    //float4 c_color;
    //float3 c_pos;
    //float c_ratio;
    //float3 c_direction;
    // ambient light
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 nrm : NORMAL;
    float4 worldPos : POSITION;
    float4 camPos : CAMPOS;
    float2 uv : TEXCOORD;
};

float4 CreateDirectionalLight(float4 color, float3 direction, float3 surfaceNormal)
{
    //float4 light = (float4)0;
    direction = normalize(direction);
    float ratio = saturate(dot(-direction, surfaceNormal));
    //light = color * ratio;
    
    return color * ratio;
}

float4 CreatePointLight(float4 color, float3 lightpos, float radius, float3 surfacePos, float3 surfaceNormal)
{
    float4 light = (float4)0;
    
    float attenuation = 1.0f - saturate(length(lightpos - surfacePos) / radius);
    attenuation *= attenuation;
    
    float3 direction = normalize(lightpos - surfacePos);
    float ratio = saturate(dot(direction, surfaceNormal));
    
    light = color * ratio * attenuation;
    
    return light;
}

float4 CreateConeLight(float4 color, float3 lightpos, float coneRatio, float3 coneDirection, float3 surfacePos, float3 surfaceNormal)
{
    float4 light = (float4)0;
    coneDirection = normalize(coneDirection);
    float3 lightdirection = normalize(lightpos - surfacePos);
    float surfaceRatio = saturate(dot(-lightdirection, coneDirection));
    float spotFactor = (surfaceRatio > coneRatio) ? 1.0f : 0.0f;
    float lightRatio = saturate(dot(lightdirection, surfaceNormal));
    
    light = spotFactor * lightRatio * color;
    
    return light;
}

float4 main(VS_OUT input) : SV_Target
{
    float4 finalColor = (float4)0;
    finalColor += CreateDirectionalLight(d_color, d_direction, input.nrm) + float4(ambientLightColor.xyz * ambientLightColor.w, 1);
    // CreatePointLight(p_color, p_pos, p_radius, input.worldPos.xyz, input.nrm.xyz) +
    // CreateConeLight(c_color, c_pos, c_ratio, c_direction, input.worldPos.xyz, input.nrm.xyz);
    finalColor *= diffuse.Sample(samLinear, input.uv);
    return finalColor;
}