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

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}

struct VS_IN
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float4 joints : JOINTS;
    float4 weights : WEIGHTS;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 pos = float4(input.position.xyz, 1.0f);
    pos = mul(pos, world_mat);
    pos = mul(pos, view_mat);
    pos = mul(pos, proj_mat);

    output.position = pos;
    output.texcoord = input.position.xyz;

    return output;
}