#include "Lighting_Calculations.hlsli"

Texture2D diffuse : register(t0);
Texture2D normal : register(t1);
Texture2D specular : register(t2);

SamplerState filter : register(s0);


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float4 world_pos : WORLD_POS;
};

cbuffer CAMERA : register(b0)
{
    float4 camera_pos;
}

cbuffer CB_LIGHT : register(b1)
{
    float4 light_data;
    LIGHT light_array[MAXLIGHTS];
}

cbuffer CB_USETEXTURE : register(b2)
{
    bool use_texture;
    bool3 padding;
}

float4 main(PS_IN input) : SV_TARGET
{    
    float4 diff_tex = diffuse.Sample(filter, input.texcoord);
    float4 norm_tex = normal.Sample(filter, input.texcoord);
    float4 spec_tex = specular.Sample(filter, input.texcoord);
        
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float3 ambient = float3(0.0f, 0.0f, 0.0f);

    matrix tbn = matrix(
        normalize(float4(input.tangent.xyz, 0)),
        normalize(float4(input.binormal.xyz, 0)),
        normalize(float4(input.normal.xyz, 0)),
        float4(0, 0, 0, 1)
    );

    float4 normal = normalize(norm_tex * 2.0f - 1.0f);
    normal = (use_texture) ? normalize(mul(float4(normal.xyz, 0), tbn)) : normalize(input.normal);
    spec_tex.r = (use_texture) ? spec_tex.r : 1.0f;
    
    int num_lights = clamp(int(light_data.x), 0, 100);
    for (int i = 0; i < num_lights; ++i)
    {
        switch (int(light_array[i].position.w))
        {
            case AMBIENT:
                ambient += (use_texture) ? float3(light_array[i].color.xyz * diff_tex.xyz) : float3(0.3f, 0.3f, 0.3f);
                break;
            case DIRECTIONAL:
                color.xyz += AddDirectionalLight(light_array[i], input.world_pos.xyz, normal.xyz, camera_pos.xyz, spec_tex.r);
                break;
            case POINT:
                color.xyz += AddPointLight(light_array[i], input.world_pos.xyz, normal.xyz, camera_pos.xyz, spec_tex.r);
                break;
            case SPOT:
                color.xyz += AddSpotLight(light_array[i], input.world_pos.xyz, normal.xyz, camera_pos.xyz, spec_tex.r);
                break;
            default:
                break;
        }
    }
    
    color.xyz *= (use_texture) ? diff_tex.xyz : input.color;
    color.xyz += ambient.xyz;

    return saturate(float4(color.xyz, diff_tex.a));

}