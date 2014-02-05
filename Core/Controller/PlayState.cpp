#include "PlayState.h"

ViewInput input;

PlayState::PlayState(Game* _game, View* _view, Engine* _engine)
{
	game = _game;
	view = _view;
	engine = _engine;
	initialized = false;
}


PlayState::~PlayState(void)
{
}

bool PlayState::init()
{
	//Let's test some stuff:
	ViewData cube = game->ViewDataFromJson("CubeThing");
	input.objects = &cube;
	input.numObjects = 1;
	CameraClass camera;
	camera.SetPosition(XMFLOAT3(0,0,0));
	input.view = camera.QuickLook(cube.world.getTrans(),Y);
	initialized = true;

	return true;
}

void PlayState::run()
{
	if(!initialized)
		return;

	//ViewInput input = engine.Update(timer.DeltaTime());
	input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(1,0,0)),game->camx);
	input.objects[0].world.rotate(XMLoadFloat3(&XMFLOAT3(0,1,0)),game->camy);
	view->Draw(input);
	
}
