struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD0;
	float3 normal		: NORMAL;	//probably useless
	float3 tangent		: TANGENT; //same
	float3 binormal		: BINORMAL; //here too
};

struct VertexToPixel
{
	float4 position		: SV_POSITION;	// System Value Position - Has specific meaning to the pipeline!
	float2 uv			: TEXCOORD0;
};

VertexToPixel main( VertexShaderInput input )
{
	VertexToPixel output;
	output.position = float4(input.position,1.0f);
	output.uv = input.uv;
	return output;
}