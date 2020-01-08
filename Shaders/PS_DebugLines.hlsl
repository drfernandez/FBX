

struct PS_IN
{
    float4 position : SV_POSITION;
    //float4 color : COLOR;
    //float2 texcoord : TEXCOORD;
    //float4 normal : NORMAL;
    //float4 tangent : TANGENT;
    //float4 binormal : BINORMAL;
    //float4 world_pos : WORLD_POS;
};

float4 main(PS_IN input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);

}