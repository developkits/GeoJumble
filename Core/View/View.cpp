#include "View.h"
#include <sstream>

View::View(void)
{
}


View::~View(void)
{
	// Release the DX stuff
	ReleaseMacro(backBufferView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(swapChain);
	ReleaseMacro(depthStencilBuffer);

	// Restore default device settings
	if( deviceContext )
		deviceContext->ClearState();

	// Release the context and finally the device
	ReleaseMacro(deviceContext);
	ReleaseMacro(device);
}

void View::DeferredDraw(ViewInput input)
{
	//Some colors, for fun!
	const float purp[4] = {0.7f,0.0f,0.3f,1.0f};
	const float black[4] = { 0, 0, 0, 0 };
	const float blue[4] = { 0, 0, 1, 1};
	const float mag[4] = {1,0,1,1};

	deviceContext->ClearRenderTargetView(backBufferView,purp);
	deviceContext->ClearRenderTargetView(diffPassView, mag);
	deviceContext->ClearRenderTargetView(normPassView, black);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	deviceContext->OMSetRenderTargets(2,deferredTargets,depthStencilView);

	for(int i = 0; i < input.numObjects; i++)
	{
		ViewData obj = input.objects[i];

		//Set up the input for the Vertex Shader
		deviceContext->IASetInputLayout(res.VSMap[obj.vertexShader].input);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		//Update constant buffer data
		XMStoreFloat4x4(&(res.vs3dCbufferData.WorldViewProj),
			XMMatrixMultiply(
			XMMatrixMultiply(
			XMLoadFloat4x4(&(obj.world())),
			XMLoadFloat4x4(&(input.view))),
			XMLoadFloat4x4(&GetProjectionMatrix())));

		res.vs3dCbufferData.World = obj.world();

		// Update the constant buffer itself
		deviceContext->UpdateSubresource(res.vs3dCbuffer,0,NULL,&(res.vs3dCbufferData),0,0);

		deviceContext->VSSetConstantBuffers(
			0,	// Corresponds to the constant buffer's register in the vertex shader
			1, 
			&(res.vs3dCbuffer));

		//Get the vertex buffer from the Mesh and the Index buffer too!
		deviceContext->IASetVertexBuffers(0,1,&res.MeshMap[obj.mesh].vertexBuffer,&stride,&offset);
		deviceContext->IASetIndexBuffer(res.MeshMap[obj.mesh].indexBuffer,DXGI_FORMAT_R32_UINT,0);

		//Get the shaders for this object
		deviceContext->VSSetShader(res.VSMap[obj.vertexShader].shader,NULL,0);
		deviceContext->PSSetShader(res.PSMap["DeferredPixelShader"].shader, NULL, 0);

		//Set the textures for the Pixel Shader
		deviceContext->PSSetShaderResources(0,res.TextureMap[obj.texture].numTextures,res.TextureMap[obj.texture].textures);

		//Finally, draw:
		deviceContext->DrawIndexed(res.MeshMap[obj.mesh].numIndeces,0,0);
	}

	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	deviceContext->OMSetRenderTargets(1,&backBufferView,depthStencilView);
	deviceContext->IASetInputLayout(res.VSMap["WindowVertexShader"].input);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0,1,&res.MeshMap["WindowPlane"].vertexBuffer,&stride,&offset);
	deviceContext->IASetIndexBuffer(res.MeshMap["WindowPlane"].indexBuffer,DXGI_FORMAT_R32_UINT,0);
	deviceContext->VSSetShader(res.VSMap["WindowVertexShader"].shader,NULL,0);
	deviceContext->PSSetShader(res.PSMap["BasicPostProcPixelShader"].shader, NULL, 0);
	deviceContext->PSSetShaderResources(0,2,deferredTextures);
	deviceContext->DrawIndexed(res.MeshMap["WindowPlane"].numIndeces,0,0);

	HR(swapChain->Present(0,0))
}

void View::Draw( ViewInput input)
{
	const float color[4] = {0.7f,0.0f,0.3f,1.0f};
	deviceContext->ClearRenderTargetView(backBufferView,color);
	deviceContext->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	for(int i = 0; i < input.numObjects; i++)
	{
		ViewData obj = input.objects[i];

		//Set up the input for the Vertex Shader
		deviceContext->IASetInputLayout(res.VSMap[obj.vertexShader].input);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		//Update constant buffer data
		XMStoreFloat4x4(&(res.vs3dCbufferData.WorldViewProj),
			XMMatrixMultiply(
				XMMatrixMultiply(
					XMLoadFloat4x4(&(obj.world())),
					XMLoadFloat4x4(&(input.view))),
				XMLoadFloat4x4(&GetProjectionMatrix())));

		res.vs3dCbufferData.World = obj.world();

		// Update the constant buffer itself
		deviceContext->UpdateSubresource(res.vs3dCbuffer,0,NULL,&(res.vs3dCbufferData),0,0);

		deviceContext->VSSetConstantBuffers(
			0,	// Corresponds to the constant buffer's register in the vertex shader
			1, 
			&(res.vs3dCbuffer));

		//Get the vertex buffer from the Mesh and the Index buffer too!
		deviceContext->IASetVertexBuffers(0,1,&res.MeshMap[obj.mesh].vertexBuffer,&stride,&offset);
		deviceContext->IASetIndexBuffer(res.MeshMap[obj.mesh].indexBuffer,DXGI_FORMAT_R32_UINT,0);

		//Get the shaders for this object
		deviceContext->VSSetShader(res.VSMap[obj.vertexShader].shader,NULL,0);
		deviceContext->PSSetShader(res.PSMap[obj.pixelShader].shader, NULL, 0);

		//Set the textures for the Pixel Shader
		deviceContext->PSSetShaderResources(0,res.TextureMap[obj.texture].numTextures,res.TextureMap[obj.texture].textures);

		//Finally, draw:
		deviceContext->DrawIndexed(res.MeshMap[obj.mesh].numIndeces,0,0);

		HR(swapChain->Present(0,0))
	}
}

#pragma region Initialization

bool View::Init(HINSTANCE hInstance, WNDPROC mainWndProc)

{
	hAppInst = hInstance;
	windowCaption = L"DirectX Game";
	driverType = D3D_DRIVER_TYPE_HARDWARE;
	windowWidth = 800;
	windowHeight = 600;
	enable4xMsaa = false;
	hMainWnd = 0;
	minimized = false;
	maximized = false;
	resizing = false;
	msaa4xQuality = 0;

	device = 0;
	deviceContext = 0;
	swapChain = 0;
	depthStencilBuffer = 0;
	diffPassView = 0;
	normPassView = 0;
	posPassView = 0;
	diffTextView = 0;
	normTextView = 0;
	posTextView = 0;
    backBufferView = 0;
	depthStencilView = 0;

	// Zero out the viewport struct
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	if(!InitWindow(mainWndProc))
		return 0;
	if(!InitDirect3D())
		return 0;
	
	InitDeferredRendering();
	InitConstBuffers();

	return true;
}

bool View::InitWindow(WNDPROC mainWndProc)
{
	// Actually create the window
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = mainWndProc;  // Can't be a member function!  Hence our global version
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if( !RegisterClass(&wc) )
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width  = R.right - R.left;
	int height = R.bottom - R.top;

	hMainWnd = CreateWindow(L"D3DWndClassName", windowCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hAppInst, 0); 
	if( !hMainWnd )
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(hMainWnd, SW_SHOW);
	UpdateWindow(hMainWnd);

	return true;
}

bool View::InitDirect3D()
{
	UINT createDeviceFlags = 0;

	// Do we want a debug device?
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the device and determine the supported feature level
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,
		driverType,
		0,
		createDeviceFlags,
		0,
		0,
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&deviceContext);

	// Handle any device creation or DirectX version errors
	if( FAILED(hr) )
	{
		MessageBox(0, L"D3D11CreateDevice Failed", 0, 0);
		return false;
	}

	if( featureLevel != D3D_FEATURE_LEVEL_11_0 )
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported", 0, 0);
		return false;
	}

	// Check for 4X MSAA quality support
	HR(device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&msaa4xQuality));
	assert( msaa4xQuality > 0 ); // Potential problem if quality is 0

	// Set up a swap chain description
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width		= windowWidth;
	swapChainDesc.BufferDesc.Height		= windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling	= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount			= 1;
	swapChainDesc.OutputWindow			= hMainWnd;
	swapChainDesc.Windowed				= true;
	swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags					= 0;
	if( enable4xMsaa )
	{
		// Set up 4x MSAA
		swapChainDesc.SampleDesc.Count   = 4;
		swapChainDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		// No MSAA
		swapChainDesc.SampleDesc.Count   = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	// To correctly create the swap chain, we must use the IDXGIFactory that
	// was used to create the device.
	IDXGIDevice*	dxgiDevice	= 0;
	IDXGIAdapter*	dxgiAdapter = 0;
	IDXGIFactory*	dxgiFactory = 0;
	HR(device->QueryInterface(	__uuidof(IDXGIDevice),	(void**)&dxgiDevice));
	HR(dxgiDevice->GetParent(	__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
	HR(dxgiAdapter->GetParent(	__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	// Finally make the swap chain and release the DXGI stuff
	HR(dxgiFactory->CreateSwapChain(device, &swapChainDesc, &swapChain));
	ReleaseMacro(dxgiDevice);
	ReleaseMacro(dxgiAdapter);
	ReleaseMacro(dxgiFactory);

	// The remaining steps also need to happen each time the window
	// is resized, so just run the OnResize method
	OnResize();

	return true;
}

bool View::InitDeferredRendering()
{
	ReleaseMacro(diffPassView);
	ReleaseMacro(normPassView);
	ReleaseMacro(diffTextView);
	ReleaseMacro(normTextView);

	string name = "DeferredPixelShader";
	LoadPixelShaders(&name,1);
	name = "BasicPostProcPixelShader";
	LoadPixelShaders(&name,1);
	name = "WindowVertexShader";
	LoadVertexShaders(&name,1);
	name = "WindowPlane";
	LoadMesh(name,false);

	ID3D11Texture2D* diffTexture;
	ID3D11Texture2D* normTexture;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	//First, the diffuse texture
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HR(device->CreateTexture2D(&textureDesc, 0, &diffTexture));

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	//Now initialize the render target
	HR(device->CreateRenderTargetView(diffTexture,&renderTargetViewDesc,&diffPassView));

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	HR(device->CreateShaderResourceView(diffTexture, &shaderResourceViewDesc, &diffTextView));

	//Next, the normal texture
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
	shaderResourceViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.Format = textureDesc.Format;
	HR(device->CreateTexture2D(&textureDesc,0,&normTexture));
	HR(device->CreateRenderTargetView(normTexture,&renderTargetViewDesc,&normPassView));
	HR(device->CreateShaderResourceView(normTexture,&shaderResourceViewDesc,&normTextView));

	deferredTargets = new ID3D11RenderTargetView*[2];
	deferredTargets[0] = diffPassView;
	deferredTargets[1] = normPassView;

	deferredTextures = new ID3D11ShaderResourceView*[2];
	deferredTextures[0] = diffTextView;
	deferredTextures[1] = normTextView;
	return true;
}

bool View::InitConstBuffers()
{
	// Constant buffers ----------------------------------------
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth			= sizeof((res.vs3dCbufferData));
	cBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags		= 0;
	cBufferDesc.MiscFlags			= 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&(res.vs3dCbuffer)));


	res.vs3dCbufferData.windowHeight = windowHeight;
	res.vs3dCbufferData.windowWidth = windowWidth;

	D3D11_BUFFER_DESC cBufferDesc2;
	cBufferDesc2.ByteWidth			= sizeof((res.psNormCbufferData));
	cBufferDesc2.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc2.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc2.CPUAccessFlags		= 0;
	cBufferDesc2.MiscFlags			= 0;
	cBufferDesc2.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc2,
		NULL,
		&(res.psNormCbuffer)));
	
	res.psNormCbufferData.diffuseColor = XMFLOAT4(1.0f,1.0f,1.0f,1.0f);
	res.psNormCbufferData.lightDirection = XMFLOAT3(0.0f,-1.0f,0.0f);
	deviceContext->UpdateSubresource(res.psNormCbuffer,0,NULL,&(res.psNormCbufferData),0,0);
	deviceContext->PSSetConstantBuffers(0,1,&(res.psNormCbuffer));
	
	return true;
}

#pragma endregion

void View::CalculateFrameStats( float totalTime )
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (totalTime - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1

		float mspf = 1000.0f / fps;

		std::wostringstream outs;  
		outs.precision(6);
		outs << windowCaption << L"    "
			<< L"Width: " << windowWidth << L"    "
			<< L"Height: " << windowHeight << L"    "
			<< L"FPS: " << fps << L"    " 
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(hMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


#pragma region Window Resizing

// Calculates the current aspect ratio
float View::AspectRatio() const
{
	return (float)windowWidth / windowHeight;
}

DirectX::XMFLOAT4X4 View::GetProjectionMatrix()
{
	if(projectionDirty)
	{
		XMStoreFloat4x4(&projection,XMMatrixPerspectiveFovLH(XM_PIDIV4,AspectRatio(),1,1000));
	}

	return projection;
}

void View::OnResize()
{
	// Release the views, since we'll be destroying
	// the corresponding buffers.
	ReleaseMacro(backBufferView);
	ReleaseMacro(depthStencilView);
	ReleaseMacro(depthStencilBuffer);

	// Resize the swap chain to match the window and
	// recreate the render target view
	HR(swapChain->ResizeBuffers(
		1, 
		windowWidth, 
		windowHeight, 
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0));
	ID3D11Texture2D* backBuffer;
	HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(device->CreateRenderTargetView(backBuffer, 0, &backBufferView));
	ReleaseMacro(backBuffer);

	// Set up the description of the texture to use for the
	// depth stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width			= windowWidth;
	depthStencilDesc.Height			= windowHeight;
	depthStencilDesc.MipLevels		= 1;
	depthStencilDesc.ArraySize		= 1;
	depthStencilDesc.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;
	if( enable4xMsaa )
	{
		// Turn on 4x MultiSample Anti Aliasing
		// This must match swap chain MSAA values
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = msaa4xQuality - 1;
	}
	else
	{
		// No MSAA
		depthStencilDesc.SampleDesc.Count   = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	// Create the depth/stencil buffer and corresponding view
	HR(device->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer));
	HR(device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView));

	// Bind these views to the pipeline, so rendering actually
	// uses the underlying textures
	deviceContext->OMSetRenderTargets(1, &backBufferView, depthStencilView);

	InitDeferredRendering();

	// Update the viewport and set it on the device
	viewport.TopLeftX	= 0;
	viewport.TopLeftY	= 0;
	viewport.Width		= (float)windowWidth;
	viewport.Height		= (float)windowHeight;
	viewport.MinDepth	= 0.0f;
	viewport.MaxDepth	= 1.0f;
	deviceContext->RSSetViewports(1, &viewport);

	res.vs3dCbufferData.windowHeight = windowHeight;
	res.vs3dCbufferData.windowWidth = windowWidth;
	projectionDirty = true;
}

#pragma endregion

#pragma region MSG Proc Handles

int View::handleWM_SIZE(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Save the new client area dimensions.
	windowWidth  = LOWORD(lParam);
	windowHeight = HIWORD(lParam);
	if( device )
	{
		if( wParam == SIZE_MINIMIZED )
		{
			//gamePaused = true;
			minimized = true;
			maximized = false;
		}
		else if( wParam == SIZE_MAXIMIZED )
		{
			//gamePaused = false;
			minimized = false;
			maximized = true;
			OnResize();
		}
		else if( wParam == SIZE_RESTORED )
		{
			// Restoring from minimized state?
			if( minimized )
			{
				//gamePaused = false;
				minimized = false;
				OnResize();
			}

			// Restoring from maximized state?
			else if( maximized )
			{
				//gamePaused = false;
				maximized = false;
				OnResize();
			}
			else if( resizing )
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				OnResize();
			}
		}
	}
	return 0;
}


int View::handleWM_ENTERSIZEMOVE( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	resizing = true;
	return 0;
}

int View::handleWM_EXITSIZEMOVE( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	resizing  = false;
	OnResize();
	return 0;
}

#pragma endregion

#pragma region Loading Stuff

bool View::LoadMesh(string filename, bool jsonFile)
{
	if(res.MeshMap.find(filename) == res.MeshMap.end())
	{
		ofstream ofs;
		ofs.open("../Resources/output.txt");

		ofs << "hey, this works\n";

		if(!jsonFile)//if this is not from a json file
		{
			ofs << "realized it wasnt json\n";
			ifstream ifs;

			char input;
			int i;

			Mesh m;

			int vertexCount; // store in the mesh struct

			ifs.open("Resources/Meshes/" + filename + ".txt");//open specified file

			if(ifs.fail())//check if it opened properly
			{
				ofs << "failed\n";
				ofs.close();
				return false;
			}
			ofs << "did not fail\n";
			ifs.get(input);
			while(input != ':')
			{
				ifs.get(input); //lets harvest all that input for the vertex count
			}

			ifs >> vertexCount; //now that we have the specified vertex count lets store it in here

			m.numIndeces = vertexCount;


			Vertex* vertices = new Vertex[vertexCount];
			UINT* indices = new UINT[vertexCount];


			ifs.get(input);
			while(input != ':')
			{
				ifs.get(input); //lets just get to the end of "data:"
			}

			ifs.get(input);//given the format lets skip ahead twice to get to all the data we want
			ifs.get(input);

				// Read in the vertex data.
			for(i=0; i<vertexCount; i++)
			{
				indices[i] = i;
				ifs >> vertices[i].xyz.x >> vertices[i].xyz.y >> vertices[i].xyz.z;
				ifs >> vertices[i].uv.x >> vertices[i].uv.y;
				ifs >> vertices[i].normalxyz.x >> vertices[i].normalxyz.y >> vertices[i].normalxyz.z;

				//For every 3 vertices, we calculate the tangent and binormal
				if(i%3 == 2)
				{
					CalculateTangentBinormal(&vertices[i-2],&vertices[i-1],&vertices[i]);
				}
			}

			// Create the vertex buffer
			D3D11_BUFFER_DESC vbd;
			vbd.Usage					= D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth				= sizeof(Vertex) * vertexCount; // Number of vertices
			vbd.BindFlags				= D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags			= 0;
			vbd.MiscFlags				= 0;
			vbd.StructureByteStride		= 0;
			D3D11_SUBRESOURCE_DATA initialVertexData;
			initialVertexData.pSysMem	= vertices;
			HR(device->CreateBuffer(&vbd, &initialVertexData, &m.vertexBuffer));

			// Create the index buffer
			D3D11_BUFFER_DESC ibd;
			ibd.Usage					= D3D11_USAGE_IMMUTABLE;
			ibd.ByteWidth				= sizeof(UINT) * vertexCount; // Number of indices
			ibd.BindFlags				= D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags			= 0;
			ibd.MiscFlags				= 0;
			ibd.StructureByteStride		= 0;
			D3D11_SUBRESOURCE_DATA initialIndexData;
			initialIndexData.pSysMem	= indices;
			HR(device->CreateBuffer(&ibd, &initialIndexData, &m.indexBuffer));

			res.MeshMap[filename] = m;
		}
		ofs.close();
	}
}

void View::LoadTextures(string* names, int numTextures, bool ddsTexture = true)
{
	ID3D11ShaderResourceView** textures = new ID3D11ShaderResourceView*[numTextures];
	for(int i = 0; i < numTextures; i++)
	{	
		if(ddsTexture)
		{
			string filename = "Resources/Textures/" + names[i] + ".dds";
			wstring wname = wstring(filename.begin(),filename.end());
			HR(CreateDDSTextureFromFile(device,wname.c_str(),nullptr,&textures[i]));
		}
		else
		{
			string filename = "Resources/Textures/" + names[i] + ".png"; //Cuz fuck you if you don't use a PNG
			wstring wname = wstring(filename.begin(),filename.end());
			HR(CreateWICTextureFromFile(device,deviceContext,wname.c_str(),nullptr,&textures[i]))
		}
	}
	Texture texture;
	texture.numTextures = numTextures;
	texture.textures = textures;
	res.TextureMap[names[0]] = texture;
	res.textureCount++;
}

void View::LoadVertexShaders( string* names, int numShaders )
{
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	for(int i = 0; i < numShaders; i++)
	{
		if(res.VSMap.find(names[i]) == res.VSMap.end())
		{
			VertexShader vs;
			string name = names[i] + ".cso";
			wstring wname = wstring(name.begin(), name.end());
			ID3DBlob* vsBlob;
			D3DReadFileToBlob(wname.c_str(), &vsBlob);

			//Create vertex shader
			HR(device->CreateVertexShader(
				vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(),
				NULL,
				&vs.shader));

			//Create input layout
			HR(device->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(),
				&vs.input));

			res.VSMap[names[i]] = vs;

			ReleaseMacro(vsBlob);
		}
	}

	res.vsCount += numShaders;
}

void View::LoadPixelShaders( string* names, int numShaders )
{
	for(int i = 0; i < numShaders; i++)
	{
		if(res.PSMap.find(names[i]) == res.PSMap.end())
		{
			PixelShader ps;
			string name = names[i] + ".cso";
			wstring wname = wstring(name.begin(), name.end());

			ID3DBlob* psBlob;
			D3DReadFileToBlob(wname.c_str(),&psBlob);

			//Create pixel shader
			HR(device->CreatePixelShader(
				psBlob->GetBufferPointer(),
				psBlob->GetBufferSize(),
				NULL,
				&ps.shader));

			res.PSMap[names[i]] = ps;

			ReleaseMacro(psBlob);
		}
	}
}

//Inspired by code at: http://www.rastertek.com/dx11tut20.html
void View::CalculateTangentBinormal( Vertex* v1, Vertex* v2, Vertex* v3 )
{
	XMFLOAT3 vector1, vector2,tangent,binormal;
	XMFLOAT2 uvector, vvector;
	float den, length;

	//Calculate vectors for this face
	vector1.x = v2->xyz.x - v1->xyz.x;
	vector1.y = v2->xyz.y - v1->xyz.y;
	vector1.z = v2->xyz.z - v1->xyz.z;

	vector2.x = v3->xyz.x - v1->xyz.x;
	vector2.y = v3->xyz.y - v1->xyz.y;
	vector2.z = v3->xyz.z - v1->xyz.z;

	//Calculate the UV space vectors
	uvector.x = v2->uv.x - v1->uv.x;
	vvector.x = v2->uv.y - v1->uv.y;

	uvector.y = v3->uv.x - v1->uv.x;
	vvector.y = v3->uv.y - v1->uv.y;

	//Calculate the denominator of the tangent/binormal equation
	den = 1.0f/(uvector.x * vvector.y - uvector.y * vvector.x);

	//Calculate the cross products and multiply by the coefficient to get the tangent and binomial
	tangent.x = (vvector.y * vector1.x - vvector.x * vector2.x) * den;
	tangent.y = (vvector.y * vector1.y - vvector.x * vector2.y) * den;
	tangent.z = (vvector.y * vector1.z - vvector.x * vector2.z) * den;

	binormal.x = (uvector.x * vector2.x - uvector.y * vector1.x) * den;
	binormal.y = (uvector.x * vector2.y - uvector.y * vector1.y) * den;
	binormal.z = (uvector.x * vector2.z - uvector.y * vector1.z) * den;

	//Normalize each vector
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	//Store them in the vertices
	v1->tangent = tangent;
	v2->tangent = tangent;
	v3->tangent = tangent;

	v1->binormal = binormal;
	v2->binormal = binormal;
	v3->binormal = binormal;
}

#pragma endregion