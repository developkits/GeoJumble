#include "PlayState.h"

ViewInput input;
float camx = 0.0f, camy = 0.0f;

PlayState::PlayState(View* _view)
{
	view = _view;
	initialized = false;
}


PlayState::~PlayState(void)
{
}

bool PlayState::init()
{
	
	ViewData* cube = ViewDataFromJson("CubeThing");
	input.objects = cube;
	input.numObjects = 1;
	CameraClass camera;
	camera.SetPosition(XMFLOAT3(0,0,0));
	input.view = camera.QuickLook(cube->world.getTrans(),Y);

	initialized = true;

	return true;
}

PlayState::PlayState(const State* x)
{
	//engine = x->getEngine();
	view = x->getView();
	initialized = false;
}

void PlayState::run(float deltaT)
{
	if(!initialized)
		return;

	input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(1,0,0)),camx*deltaT);
	input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(0,1,0)),camy*deltaT);
	
	view->Draw(input);
	
}

void PlayState::processKeyboardInput(BYTE input[256])
{
	float MAX_ROTATE = 5;

	//Camx and Camy are flipped. Dunno why?
	if(input[DIK_LEFT] & 0x80 && abs(camy) < MAX_ROTATE)
	{
		camy -= 0.01;
	}
	else
		if(camy < 0)
			camy += 0.005;

	if(input[DIK_RIGHT] & 0x80 && abs(camy) < MAX_ROTATE)
	{
		camy += 0.01;
		
	}
	else
		if(camy > 0)
			camy -= 0.005;
	if(input[DIK_UP] & 0x80 && abs(camx) < MAX_ROTATE)
	{
		camx += 0.01;
		
	}
	else
		if(camx > 0)
			camx -= 0.005;
	if(input[DIK_DOWN] & 0x80 && abs(camy) < MAX_ROTATE)
	{
		camx -= 0.01;
		
	}
	else
		if(camx < 0)
			camx += 0.005;
}

void PlayState::processMouseInput(POINT di)
{
}