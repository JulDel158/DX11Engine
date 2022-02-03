Texture2D diffuse : register(t0);
SamplerState samLinear : register(s0);

// THIS DATA IS BEING BOUNDED TO THE VERTEX SHADER, NOT THE PIXEL SHADER
//cbuffer Object_cb : register(b0) // binder per object per frame
//{
//    matrix modeling;
//};

//cbuffer Frame_cb : register(b1) // binded once every frame
//{
//    matrix view;
//};

//cbuffer Window_cb : register(b2) // binded whenever the window changes
//{
//    matrix projection;
//};

struct PointLight
{
    float4 color;
    float3 pos;
    float radius;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
};

struct SpotLight
{
    float4 color;
    float3 pos;
    float innerRatio;
    float3 direction;
    float outerRatio;
    float range;
    float focus;
    float falloff;
};

cbuffer Scene_cb : register(b3)
{
    PointLight plight;
    DirectionalLight dlight;
    SpotLight slight;
    float4 ambientLightColor;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 nrm : NORMAL;
    float3 worldPos : POSITION;
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

float4 CreateSpotLight(float4 color, float3 lightpos, float outerRatio, float innerRatio, float falloff, float3 coneDirection, float range, float focus, float3 surfacePos, float3 surfaceNormal)
{
    float4 light = (float4)0;
    coneDirection = normalize(coneDirection);
    
    // not working...
    //float3 lightdirection = normalize(lightpos - surfacePos);
    //float surfaceRatio = saturate(dot(-lightdirection, coneDirection));
    //float spotFactor = (surfaceRatio > innerRatio) ? 1.0f : 0.0f;
    ////float attenuation = 1.0 - saturate((innerRatio - surfaceRatio) / (innerRatio - outerRatio));
    ////attenuation *= attenuation;
    //float lightRatio = normalize(dot(lightdirection, surfaceNormal));
    //light = spotFactor * lightRatio * color;
    
    float3 pixelToLight = (lightpos - surfacePos);
    float d = length(pixelToLight);
    if (d <= range)
    {
        pixelToLight = normalize(pixelToLight);
        float hmlight = dot(pixelToLight, surfaceNormal);
        if (hmlight > 0.0f)
        {
            light = color;
            light /= (outerRatio + (innerRatio * d)) + (falloff * (d * d));
            light *= pow(max(dot(-pixelToLight, coneDirection), 0.0f), focus);
        }

    }
    
    return light;
}

float4 main(VS_OUT input) : SV_Target
{
    float4 finalColor = (float4)0;
    finalColor += CreateDirectionalLight(dlight.color, dlight.direction, input.nrm) +
    CreatePointLight(plight.color, plight.pos, plight.radius, input.worldPos, input.nrm) +
    CreateSpotLight(slight.color, slight.pos, slight.outerRatio, slight.innerRatio, 
                    slight.falloff, slight.direction, slight.range, slight.focus,
                    input.worldPos, input.nrm) +
    float4(ambientLightColor.xyz * ambientLightColor.a, 1);
    // CreatePointLight(p_color, p_pos, p_radius, input.worldPos.xyz, input.nrm.xyz) +
    // CreateConeLight(c_color, c_pos, c_ratio, c_direction, input.worldPos.xyz, input.nrm.xyz);
    finalColor *= diffuse.Sample(samLinear, input.uv);
    
    return saturate(finalColor);
}