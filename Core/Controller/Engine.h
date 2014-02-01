#pragma once
#include <string.h>
#include "Actor.h"
#include <vector>
#include "State.h"

//Starting point for dealing with ONLY gameplay code
class Engine : public State
{
public:
	unsigned int counter;
	Actor *user;
	vector<Particle*> particleArr;
	int arrayIndex;
	unsigned int reservedAmount;
	unsigned int enemyCounter;
	unsigned int enemyTimerCounter;

	bool holdingUp;
	bool holdingDown;
	bool holdingLeft;
	bool holdingRight;
	bool holdingAttack;

	float magnitude;

	Engine(View* view);
	~Engine(void);
	void run(float deltaTime);


	bool init();
	void processKeyboardInput(BYTE input[256]);
	void processMouseInput(POINT di);

	XMMATRIX inverseViewProjection;
};

