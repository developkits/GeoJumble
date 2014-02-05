#include "Game.h"
#include "Camera.h"

class State
{
public:
	virtual bool init() =0;
	virtual void run() =0;
	bool initialized;

protected:
	Game* game;
	View* view;
	Engine* engine;
};