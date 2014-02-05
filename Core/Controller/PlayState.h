#pragma once
#include "State.h"

class PlayState :
	public State
{
public:
	PlayState(Game* _game, View* _view, Engine* _engine);
	~PlayState(void);
	virtual bool init();
	virtual void run();
};

