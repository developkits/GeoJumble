#pragma  once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct Mesh
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndeces;
};

struct Texture
{
	ID3D11ShaderResourceView** textures;
	int numTextures;
};

struct VertexShader
{
	ID3D11VertexShader* shader;
	ID3D11InputLayout* input;
};

struct PixelShader
{
	ID3D11PixelShader* shader;
};

struct Vertex
{
	XMFLOAT3 xyz;
	XMFLOAT2 uv;
	XMFLOAT3 normalxyz;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
};