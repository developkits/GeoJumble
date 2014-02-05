#include "Game.h"
#include <WindowsX.h>
#include <d3dcompiler.h>
#include "Camera.h"
#include <dinput.h>
#include <initguid.h>
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma region Global Window Callback
namespace
{
	// Allows us to forward Windows messages from a global
	// window procedure to our member function window procedure
	// because we cannot assign a member function to WNDCLASS::lpfnWndProc
	Game* game = 0;
}

// Set up a global callback for handling windows messages
LRESULT CALLBACK
	MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward the global callback to our game's message handling

	// Forward hwnd because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return game->MsgProc(hwnd, msg, wParam, lParam);
}
#pragma endregion

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Make the game, initialize and run
	Game game;
	if( !game.Init(hInstance) )
		return 0;

	return game.Run();
}

#pragma endregion

Game::Game(void)
{
	game = this;
}


Game::~Game(void)
{
	delete view;
}

bool Game::Init( HINSTANCE hInstance )
{
	
	view = new View();
	game->InitDirectInput(hInstance);
	//gameStates = list<State>();
	//PlayState mainState = PlayState(game, view, &engine);
	
	//if(!mainState.init())
	//	return 0;

	//gameStates.push_front(mainState);

	return view->Init(hInstance,MainWndProc);
}

int Game::Run()
{
	//Let's test some stuff:
	ViewData cube = ViewDataFromJson("CubeThing");
	ViewInput input;
	input.objects = &cube;
	input.numObjects = 1;
	CameraClass camera;
	camera.SetPosition(XMFLOAT3(0,0,0));
	input.view = camera.QuickLook(cube.world.getTrans(),Y);
	

	MSG msg = {0};
	timer.Reset();

	// Loop until we get a quit message from windows
	while(msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// No message, so continue the game loop
			timer.Tick();

			if( gamePaused )
			{
				Sleep(100);
			}
			else
			{
				// Standard game loop type stuff
				view->CalculateFrameStats(timer.TotalTime());
				game->DetectInput(timer.TotalTime());
				//ViewInput input = engine.Update(timer.DeltaTime());
				input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(1,0,0)),camx * timer.DeltaTime());
				input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(0,1,0)),camy * timer.DeltaTime());
				view->Draw(input);
				//gameStates.front().run();
			}
		}
	}
	//free(cube);

	return (int)msg.wParam;
}

//This will return a ViewData struct for an object by getting its object name
ViewData Game::ViewDataFromJson( string objectName )
{
	JsonUtils reader;
	string filename = "Resources/Objects/" + objectName;
	JsonData data = reader.ReadJsonFromFile(filename);
	ViewData toReturn;

	toReturn.mesh = data.GetValue("Mesh").asString();
	view->LoadMesh(toReturn.mesh,false);

	toReturn.vertexShader = data.GetValue("VertexShader").asString();
	view->LoadVertexShaders(&(toReturn.vertexShader),1);

	toReturn.pixelShader = data.GetValue("PixelShader").asString();
	view->LoadPixelShaders(&(toReturn.pixelShader),1);

	Json::Value txts = data.GetValue("Textures");
	string* textures = new string[txts.size()];
	for(int i = 0; i < txts.size(); i++)
	{
		textures[i] = txts[i].asString();
	}
	toReturn.texture = textures[0];
	view->LoadTextures(textures,txts.size(),false);

	float tX = data.GetValue("World.Translation.x").asFloat();
	float tY = data.GetValue("World.Translation.y").asFloat();
	float tZ = data.GetValue("World.Translation.z").asFloat();
	float sX = data.GetValue("World.Scale.x").asFloat();
	float sY = data.GetValue("World.Scale.y").asFloat();
	float sZ = data.GetValue("World.Scale.z").asFloat();
	float rX = data.GetValue("World.Rotation.x").asFloat();
	float rY = data.GetValue("World.Rotation.y").asFloat();
	float rZ = data.GetValue("World.Rotation.z").asFloat();

	World world(tX,tY,tZ,sX,sY,sZ,rX,rY,rZ);

	toReturn.world = world;

	return toReturn;
}


bool Game::InitDirectInput(HINSTANCE hInstance)
{
	game->camx = 0.0;
	game->camy = 0.0;
	game->camz = 0.0;

	
	game->hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&(game->DirIn),
		NULL); 

	game->hr = game->DirIn->CreateDevice(GUID_SysKeyboard,
		&game->IKeyboard,
		NULL);

	game->hr = game->DirIn->CreateDevice(GUID_SysMouse,
		&game->IMouse,
		NULL);

	game->hr = game->IKeyboard->SetDataFormat(&c_dfDIKeyboard);
	game->hr = game->IKeyboard->SetCooperativeLevel(game->view->hMainWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	game->hr = game->IMouse->SetDataFormat(&c_dfDIMouse);
	game->hr = game->IMouse->SetCooperativeLevel(game->view->hMainWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	return true;
}

void Game:: DetectInput(double time)
{
	DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	game->IKeyboard->Acquire();
	game->IMouse->Acquire();

	game->IMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	game->IKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

	if(keyboardState[DIK_ESCAPE] & 0x80)
		PostMessage(game->view->hMainWnd, WM_DESTROY, 0, 0);

	if(keyboardState[DIK_LEFT] & 0x80)
	{
		game->camx -= 0.01;
	}
	if(keyboardState[DIK_RIGHT] & 0x80)
	{
		game->camx += 0.01;
		
	}
	if(keyboardState[DIK_UP] & 0x80)
	{
		game->camy += 0.01;
		
	}
	if(keyboardState[DIK_DOWN] & 0x80)
	{
		game->camy -= 0.01;
		
	}
	if(mouseCurrState.lX != game->mouseState.lX)
	{
		game->ScaleX -= (mouseCurrState.lX * 0.001f);
	}
	if(mouseCurrState.lY != game->mouseState.lY)
	{
		game->ScaleY -= (mouseCurrState.lY * 0.001f);
	}


	game->mouseState = mouseCurrState;

	return;
}



void Game::InputCleanUp()
{
	
	game->IKeyboard->Unacquire();
	game->IMouse->Unacquire();
	game->DirIn->Release();
	
}






#pragma region Windows Message Processing

LRESULT Game::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			gamePaused = true;
			timer.Stop();
		}
		else
		{
			gamePaused = false;
			timer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		return view->handleWM_SIZE(hwnd,msg,wParam,lParam);

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		gamePaused = true;
		view->handleWM_ENTERSIZEMOVE(hwnd,msg,wParam,lParam);
		timer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		gamePaused = false;
		timer.Start();
		return view->handleWM_EXITSIZEMOVE(hwnd,msg,wParam,lParam);

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		//OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#pragma endregion