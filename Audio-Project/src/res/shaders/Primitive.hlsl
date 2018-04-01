/*
    Basic VS/PS shaders for a primitive
*/

struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
};

cbuffer cbData : register(b0)
{
    float4x4 WVP;
}

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output;

    output.position = mul(float4(input.position, 1.f), WVP);
    output.normal = input.normal;
    return output;
}

float4 PS_MAIN(VS_OUT input) : SV_Target
{
    return float4(1.f, 0.f, 0.f, 1.f);
}