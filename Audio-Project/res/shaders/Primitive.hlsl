/*
    Basic VS/PS shaders with Phong illumination based on a directional light source for a primitive
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
    float3 fragPos : TEXCOORD1;
};

cbuffer cbData : register(b0)
{
    float4x4 WVP;
    float4x4 world;
}

cbuffer lightCbData : register(b1)
{
    float3 lightPosition;
    float ambientStrength;
    float3 camPos;
    float specularStrength;
    float3 color;
}

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output;

    output.position = mul(float4(input.position, 1.f), WVP);
    output.normal = mul(input.normal, (float3x3)world); //Get the normal in world space
    output.normal = normalize(output.normal);

    //Light
    output.fragPos = mul(float4(input.position, 1.f), world);

    return output;
}

float4 PS_MAIN(VS_OUT input) : SV_Target
{
    //Light constants
    float shininess = 32.0f;

    //Ambient
    float3 ambient = ambientStrength * color;
  	
    //Diffuse 
    float3 norm = input.normal;
    float3 lightDir = normalize(lightPosition- input.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = diff * color;

    //Specular
    float3 viewDir = normalize(camPos - input.fragPos);
    float3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float3 specular = specularStrength * spec * color;
        
    float3 phong = ambient + diffuse + specular;
    return float4(phong, 1.0f);
}