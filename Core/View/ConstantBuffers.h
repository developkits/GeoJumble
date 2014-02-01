#pragma  once
#include <d3d11.h>
#include <DirectXMath.h>

_declspec(align(16))
struct VS3dCbuffer
{
	XMFLOAT4X4 World; //Why do we includ world twice?  Because its cheaper to multiply WVP only once per model, but we still need the world matrix in our shaders
	XMFLOAT4X4 WorldViewProj;
	int windowWidth;
	int windowHeight;
};

_declspec(align(16))
struct PSNormCBuffer
{
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	int colorPreference;
};