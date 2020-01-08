#pragma pack_matrix(row_major)

cbuffer WORLD : register(b0)
{
    matrix world_mat;
};

cbuffer VIEWPROJ : register(b1)
{
    matrix view_mat;
    matrix proj_mat;
};

struct VS_IN
{
    float4 position : POSITION;
    //float4 color : COLOR;
    //float2 texcoord : TEXCOORD;
    //float4 normal : NORMAL;
    //float4 tangent : TANGENT;
    //float4 binormal : BINORMAL;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    //float4 color : COLOR;
    //float2 texcoord : TEXCOORD;
    //float4 normal : NORMAL;
    //float4 tangent : TANGENT;
    //float4 binormal : BINORMAL;
    //float4 world_pos : WORLD_POS;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 pos = float4(input.position.xyz, 1.0f);
    pos = mul(pos, world_mat);
    //output.world_pos = pos;
    pos = mul(pos, view_mat);
    pos = mul(pos, proj_mat);

    output.position = pos;
    //output.color = input.color;
    //output.texcoord = input.texcoord;

    //float4 normal = float4(input.normal.xyz, 0.0f);
    //normal = mul(normal, world_mat);

    //output.normal = normal;
    //output.tangent = mul(float4(input.tangent.xyz, 0), world_mat);
    //output.binormal = mul(float4(input.binormal.xyz, 0), world_mat);


    return output;
}