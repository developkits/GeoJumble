#pragma pack_matrix ( row_major )

// The constant buffer that holds our "per model" data
// - Each object you draw with this shader will probably have
//   slightly different data (at least for the world matrix)
cbuffer perModel : register( b0 )
{
	matrix World;
	matrix WorldViewProj;
	int windowWidth;
	int windowHeight;
};

// Defines what kind of data to expect as input
// - This should match our input layout!
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 binormal		: BINORMAL;

};

// Defines the output data of our vertex shader
// - At a minimum, you'll need an SV_POSITION
// - Should match the pixel shader's input
struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
	float4 tangent		: TANGENT;
	float4 binormal		: BINORMAL;
};

// The entry point for our vertex shader
VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;

	// Convert from local to screen space
	output.position = mul(float4(input.position, 1.0f), WorldViewProj);
	output.normal = normalize(mul(input.normal,World));
	output.tangent = normalize(mul(input.tangent,World));
	output.binormal = normalize(mul(input.binormal,World));
	output.uv = input.uv;

	return output;
}