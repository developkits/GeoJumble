#pragma pack_matrix ( row_major )

Texture2D ColorTexture[2];

cbuffer LightBuffer : register( b0 )
{
	float4 diffuseColor;
	float3 lightDirection;
}

SamplerState TrilinearSampler
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 textureColor = ColorTexture[0].Sample(TrilinearSampler,input.uv);
	float4 normal = ColorTexture[1].Sample(TrilinearSampler,input.uv);

	float diffuseIntensity = saturate(dot(normal,normalize(-lightDirection)));
	float4 color = saturate(diffuseIntensity * diffuseColor);
	color = color * textureColor;

	return color;
}