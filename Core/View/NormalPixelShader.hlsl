#pragma pack_matrix ( row_major )

Texture2D ColorTexture[2];

cbuffer LightBuffer : register( b0 )
{
	float4 diffuseColor;
	float3 lightDirection;
}

SamplerState TrilinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float4 tangent		: TANGENT;
	float4 binormal		: BINORMAL;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 textureColor = ColorTexture[0].Sample(TrilinearSampler,input.uv);
	float4 normalMap = ColorTexture[1].Sample(TrilinearSampler,input.uv);
	normalMap = -((normalMap * 2.0f) - 1.0f);
	float3 normal = normalize(input.normal + (normalMap.x * input.tangent) + (normalMap.y * input.binormal));
	float diffuseIntensity = saturate(dot(normal,normalize(-lightDirection)));
	float4 color = saturate(diffuseIntensity * diffuseColor);
	color = color * textureColor;

	return color;
}