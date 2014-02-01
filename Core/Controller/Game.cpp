#include "Game.h"
#include <WindowsX.h>
#include <d3dcompiler.h>
#include "Camera.h"
#include <dinput.h>
#include <initguid.h>
#include <ctime>
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
	srand(time(0));
	view = new View();
	game->InitDirectInput(hInstance);
	gameStates = list<State*>();

	PlayState* mainState = new PlayState(view);
	Engine* mainEngine = new Engine(view);
	

	//gameStates.push_front(mainState);
	gameStates.push_front(mainEngine);
	bool viewInit = view->Init(hInstance,MainWndProc);
	if( !gameStates.front()->init() )
		return false;

	view->getWinRect(&windowRect);

	return viewInit;
}

int Game::Run()
{


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

				//Get the iterator for our various game states and loop through their run() methods
				list<State*>::iterator iterator;
				for ( iterator = gameStates.begin(); iterator != gameStates.end(); iterator++)
				{
					(*iterator)->run(timer.DeltaTime());
				}

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
	view->LoadTextures(textures,txts.size(),true);

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


	CreateConsoleLog(L"Geojumble Console Window");

	return true;
}

void Game:: DetectInput(double time)
{
	//DIMOUSESTATE mouseCurrState;

	BYTE keyboardState[256];

	game->IKeyboard->Acquire();
	game->IMouse->Acquire();

	view->getWinRect(&windowRect);
	view->getCRect(&clientRect);
	
	//Get the cursor position in the screen
	POINT cursorScreenPos, CursorWindowPos;
	CursorWindowPos = POINT();
	GetCursorPos(&cursorScreenPos);
	
	//We have margins on the sides of our windows, so we need to use these.
	int caption_thickness = GetSystemMetrics(SM_CYCAPTION);
	int border = GetSystemMetrics(SM_CXSIZEFRAME);

	//Figure out its position in our window
	CursorWindowPos.x = (cursorScreenPos.x - windowRect.left - border);
	CursorWindowPos.y = (cursorScreenPos.y - windowRect.top - caption_thickness - border);

	//game->IMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

	game->IKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);
	
	//Iterate through our gamestates and pass them our input
	list<State*>::iterator iterator;
	for ( iterator = gameStates.begin(); iterator != gameStates.end(); iterator++)
	{
		(*iterator)->processKeyboardInput(keyboardState);
		(*iterator)->processMouseInput(CursorWindowPos);
	}

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
	/*if(mouseCurrState.lX != game->mouseState.lX)
	{
		game->ScaleX -= (mouseCurrState.lX * 0.001f);
	}
	if(mouseCurrState.lY != game->mouseState.lY)
	{
		game->ScaleY -= (mouseCurrState.lY * 0.001f);
	}*/


	//game->mouseState = mouseCurrState;

	return;
}



void Game::InputCleanUp()
{
	
	game->IKeyboard->Unacquire();
	game->IMouse->Unacquire();
	game->DirIn->Release();
	
}



void Game::CreateConsoleLog(LPCWSTR winTitle)
{
	AllocConsole();
	SetConsoleTitle(winTitle);

	int hConHandle;
	long lStdHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
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