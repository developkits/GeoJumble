#pragma once
#include <string>
#include <map>
#include "ViewStructs.h"
#include "ConstantBuffers.h"
#include <d3d11.h>

using namespace std;
class Resources
{
public:
	Resources(void);
	~Resources(void);
	map<string, Texture> TextureMap;
	map<string, Mesh> MeshMap;
	map<string, VertexShader> VSMap;
	map<string, PixelShader> PSMap;

	int textureCount;
	int meshCount;
	int vsCount;
	int psCount;


#pragma region Constant Buffers

	ID3D11Buffer* vs3dCbuffer;
	VS3dCbuffer vs3dCbufferData;
	ID3D11Buffer* psNormCbuffer;
	PSNormCBuffer psNormCbufferData;


#pragma endregion
};

