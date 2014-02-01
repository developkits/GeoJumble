#pragma pack_matrix ( row_major )

Texture2D ColorTexture[2];

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

struct PS_Output
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	//float4 somethingElse : SV_Target2;
};

// Entry point for this pixel shader
PS_Output main(VertexToPixel input)
{
	PS_Output output;
	float4 textureColor = ColorTexture[0].Sample(TrilinearSampler,input.uv);
	output.Color = textureColor;
	float4 normalMap = ColorTexture[1].Sample(TrilinearSampler,input.uv);
	normalMap = -((normalMap * 2.0f) - 1.0f);
	float3 normal = normalize(input.normal + (normalMap.x * input.tangent) + (normalMap.y * input.binormal));
	output.Normal = float4(normal,1.0f);

	return output;
}