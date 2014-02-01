#pragma once
#include "State.h"
#include "Engine.h"

class PlayState :
	public State
{
public:
	PlayState(View* _view);
	PlayState (const State* x);
	~PlayState(void);
	virtual bool init();
	virtual void run(float deltaT);
	virtual void processKeyboardInput(BYTE input[256]);
	virtual void processMouseInput(POINT di);
};

