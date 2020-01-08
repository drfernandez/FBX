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

cbuffer POSEJOINTS : register(b2)
{
    matrix curr_joint[66];
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
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float4 binormal : BINORMAL;
    float4 world_pos : WORLD_POS;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 skinned_pos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 skinned_norm = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 skinned_tangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 skinned_binormal = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (int i = 0; i < 4; ++i)
    {
        skinned_pos += mul(float4(input.position.xyz, 1.0f), curr_joint[(int) input.joints[i]]) * input.weights[i];
        skinned_norm += mul(float4(input.normal.xyz, 0.0f), curr_joint[(int) input.joints[i]]) * input.weights[i];
        skinned_tangent += mul(float4(input.tangent.xyz, 0.0f), curr_joint[(int) input.joints[i]]) * input.weights[i];
        skinned_binormal += mul(float4(input.binormal.xyz, 0.0f), curr_joint[(int) input.joints[i]]) * input.weights[i];
    }

    float4 pos = float4(skinned_pos.xyz, 1.0f);
    pos = mul(pos, world_mat);
    output.world_pos = pos;
    pos = mul(pos, view_mat);
    pos = mul(pos, proj_mat);

    output.position = pos;
    output.color = input.color;
    output.texcoord = input.texcoord;

    float4 normal = float4(skinned_norm.xyz, 0.0f);
    normal = mul(normal, world_mat);

    output.normal = normal;
    output.tangent = mul(float4(skinned_tangent.xyz, 0), world_mat);
    output.binormal = mul(float4(skinned_binormal.xyz, 0), world_mat);


    return output;
}