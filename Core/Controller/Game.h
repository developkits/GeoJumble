
#include <d3d11.h>
#include <assert.h>
#include "Macros.h"
#include "GameTimer.h"
#include "Windows.h"
#include "Engine.h"
#include "ViewStructs.h"
#include "JsonUtils.h"
#include "View.h"
#include "PlayState.h"
#include <dinput.h>
#include <initguid.h>
#include <list>
#include <io.h>
#include <fcntl.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
// Include run-time memory checking in debug builds
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif


class Game
{
public:
	GameTimer timer;
	View* view;
	bool gamePaused;
	//Engine engine(view);
	
	RECT windowRect, clientRect;

	IDirectInputDevice8* IKeyboard;
	IDirectInputDevice8* IMouse;

	DIMOUSESTATE mouseState;
	LPDIRECTINPUT8 DirIn;
	float camx;
	float camy;
	float camz;
	float ScaleX;
	float ScaleY;
	HRESULT hr;

	Game(void);
	~Game(void);
	void CreateConsoleLog(LPCWSTR winTitle);
	bool Init(HINSTANCE hInstance);
	bool InitDirectInput(HINSTANCE hInstance);
	void DetectInput(double time);
	void InputCleanUp();
	int Run();
	ViewData ViewDataFromJson(string objectName);

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	list<State*> gameStates;
};



