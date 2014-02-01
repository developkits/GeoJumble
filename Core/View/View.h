#pragma once

#include "Macros.h"

#include "Resources.h"
#include "ModelStructs.h"
#include <fstream>
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "ConstantBuffers.h"

using namespace std;

#include <DirextXTK/WICTextureLoader.h>
#include <DirextXTK/DDSTextureLoader.h>
class View
{
public:

	View(void);
	~View(void);
	void Draw(ViewInput input);
	void DeferredDraw(ViewInput input);
	bool Init(HINSTANCE hInstance, WNDPROC mainWndProc);
	float AspectRatio()const;
	void OnResize(); 
	//Functions to handle msgProcs
	int handleWM_SIZE(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int handleWM_ENTERSIZEMOVE(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int handleWM_EXITSIZEMOVE(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void CalculateFrameStats(float totalTime);
	
	Resources res;
	void LoadTextures(string* names, int numTextures, bool ddsTexture); //Load a single set of textures for a given "texture" map, including normal map, specular map, etc
	bool LoadMesh(string,bool); //Load one mesh
	void LoadVertexShaders(string* names, int numShaders); //Load as many shaders as you want
	void LoadPixelShaders(string* names, int numShaders); //Load as many shaders as you want
	void CalculateTangentBinormal(Vertex* v1, Vertex* v2, Vertex* v3); //Add in the Tangents

	// Window handles and such
	HINSTANCE hAppInst;
	HWND      hMainWnd;
	bool	  minimized;
	bool	  maximized;
	bool	  resizing;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	IDXGISwapChain* swapChain;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RenderTargetView* backBufferView;
	ID3D11DepthStencilView* depthStencilView;
	D3D11_VIEWPORT viewport;
	D3D_DRIVER_TYPE driverType;

	//Deferred rendering stuff
	ID3D11RenderTargetView* diffPassView;
	ID3D11RenderTargetView* normPassView;
	ID3D11RenderTargetView* posPassView;
	ID3D11RenderTargetView** deferredTargets;
	ID3D11ShaderResourceView* diffTextView;
	ID3D11ShaderResourceView* normTextView;
	ID3D11ShaderResourceView* posTextView;
	ID3D11ShaderResourceView** deferredTextures;

	UINT msaa4xQuality;
	std::wstring windowCaption;
	bool enable4xMsaa;
	int windowWidth;
	int windowHeight;
	XMMATRIX projectionMatrix;


	XMFLOAT4X4 GetProjectionMatrix();
	XMFLOAT4X4 projection;
	bool projectionDirty;

	bool InitWindow(WNDPROC mainWndProc);
	bool InitDirect3D();
	bool InitConstBuffers();
	bool InitDeferredRendering();

	void getWinRect(RECT*);
	void getCRect(RECT*);
};

