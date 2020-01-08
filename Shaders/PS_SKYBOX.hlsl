
TextureCube skybox : register(t0);
SamplerState filter : register(s0);

struct PS_IN
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
};

float4 main(PS_IN input) : SV_TARGET
{
    //return skybox.SampleLevel(filter, normalize(input.texcoord.xyz), 0.0f);
    return skybox.Sample(filter, input.texcoord.xyz);
}