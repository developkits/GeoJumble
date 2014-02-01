	#pragma once
#include "Engine.h"

string pMesh, pVertexShader, pPixelShader;
XMFLOAT4X4 cameraView;
bool cameraLock = false;

	/*const int DEFAULT_SCREEN_WIDTH = 800;
	const int DEFAULT_SCREEN_HEIGHT = 600;
	const int MIN_SCREEN_WIDTH = 800;
	const int MIN_SCREEN_HEIGHT = 600;*/

	const int SPAWN_DISTANCE = 500;
	const int REMOVE_DISTANCE = 1000;
	const int MAX_DROP = 500;
	const int MAX_USER_BULLET = 150;
	const int MAX_ENEMY_BULLET = 200;
	const int MAX_ENEMY = 0;
	const int ENEMY_RATE = 600;

	const int ARRAY_SIZE = 1000;
	ViewData* pointer;
	float mouseX, mouseY;

Engine::Engine(View* view)
{
	/*screenWidth = DEFAULT_SCREEN_WIDTH;
	screenHeight = DEFAULT_SCREEN_HEIGHT;*/

	/*
	in flash; this part would inititalize any 
	input and running functions

	addEventListener(Event.ENTER_FRAME, onEnterFrame);
	stage.addEventListener(KeyboardEvent.KEY_DOWN, doKeys);
	stage.addEventListener(KeyboardEvent.KEY_UP, stopKeys);
	*/

	this->view = view;

	counter = 0;
	enemyCounter = 0;
	enemyTimerCounter = 0;

	holdingAttack = false;
	holdingDown = false;
	holdingLeft = false;
	holdingRight = false;
	holdingUp = false;

	/*previousTime = 0;
	currentTime = 0;*/
	magnitude = 0;
	arrayIndex = 0;

	mouseX = 0.0;
	mouseY = 0.0;

	particleArr = vector<Particle*>(ARRAY_SIZE);
}


Engine::~Engine(void)
{
}


void Engine::run(float deltaTime)
{
	/* iff deltaTime does in fact equal time elapsed since last frame in milliseconds*/
	/*
	flash code

	currentTime = getTimer();
	difference = (currentTime - previousTime) / 1000;

	previousTime = currentTime;//update for next go around
	*/
	magnitude = (deltaTime) * 60;

	/*increment time by magnitude*/
	if(enemyTimerCounter < ENEMY_RATE)
	{
		enemyTimerCounter += magnitude;
	}
	if(enemyTimerCounter >= ENEMY_RATE && enemyCounter < MAX_ENEMY)
	{
		enemyTimerCounter = 0;
		int i = 0;
		while(i < MAX_ENEMY)
		{
			if(!particleArr[i + 1]->active)
			{
				float r = rand() * 360;
				particleArr[i + 1]->init(((cos((r * XM_PI) / 180))* 500 + 400),
																(300 - (sin((r * XM_PI) / 180))* 500),
																0,0,0,false,&particleArr,ARRAY_SIZE,reservedAmount,&arrayIndex);
				particleArr[i + 1]->setActor(user);
				particleArr[i + 1]->drawing = true;//as a precaution from switching languages
			}
			i++;
		}
	}
	int k = 0;

	ViewInput input;
	int index = 0;
	input.objects = new ViewData[arrayIndex+1];
	input.numObjects = arrayIndex+1; //index;
	
	pointer->world.tX(mouseX);
	pointer->world.tY(-mouseY);
	pointer->world.tZ(200);
	input.objects[arrayIndex] = *pointer;

	ViewData vdArray[1];

	CameraClass camera;
	camera.SetPosition(XMFLOAT3(0,0,0));
	
	while(k < arrayIndex)
	{
		if(particleArr[k]->active)
		{
			if(particleArr[k]->drawing)
			{
				//precaution from switching languages
				particleArr[k]->drawing = true;
			}

			particleArr[k]->act(magnitude);

			if(k >= reservedAmount)
			{
				if(particleArr[k]->relationship == 0)//if it is neutral
				{
					if(particleArr[k]->ownerActor == NULL && user->checkAllCollision(particleArr[k]->drawX,particleArr[k]->drawY,10,10,particleArr[k]->relationship))
					{
						if(!user->addToArray(particleArr[k]))
						{
							particleArr[k]->sendCommand("remove");
							arrayIndex--;
						}
					}
					if(particleArr[k]->timer > 3)
					{
						particleArr[k]->sendCommand("remove");
						arrayIndex--;
					}
				}
				else
				{
					if(particleArr[k]->timer > 3)//2.25 seconds
					{
						particleArr[k]->sendCommand("remove");
						arrayIndex--;
					}
					else
					{
						int j = 0;
						while(j < reservedAmount)
						{
							if(particleArr[j]->active)
							{
								if(particleArr[j]->checkAllCollision(particleArr[k]->drawX,particleArr[k]->drawY,10,10,particleArr[k]->relationship))
								{
									particleArr[k]->sendCommand("remove");
								}
							}
							j++;
						}
					}
				}
			}
		}
		else
		{
			particleArr[k]->drawing = false;
		}

		

		if(particleArr[k]->drawing && particleArr[k]->hasViewData)
		{
			/* DIRECTX HELP NEEDED HERE */	

			/*
			alright so what is supposed to happen here is that if the
			particle is "drawing" then it should be drawn to the screen

			that being said each particle has a drawX and drawY which
			determine the x and y location in the grid in 2D sense
			*/
			/* SHUT UP, ALEX. GO HOME, YOU'RE DRUNK*/
			if(arrayIndex != input.numObjects-1)
				printf("arrayIndex has changed! Skipping objects from draw.\n");
			else
			{
				particleArr[k]->syncDraw();
 
				input.objects[k] = *(particleArr[k]->getViewData());
				//vdArray[k] = *(particleArr[k]->getViewData());
				//input.numObjects++;
			}
			

		}
		k++;
	}


	if(cameraLock)
	{
		cameraView = camera.QuickLook(user->getViewData()->world.getTrans(), Y);
		input.view = camera.QuickLook(user->getViewData()->world.getTrans(),Y);
	}
	else
		input.view = cameraView;

	//input.objects = vdArray;
	view->Draw(input);

	//delete[] input.objects;
}
bool Engine::init()
{
	
	arrayIndex = MAX_ENEMY + 1;
	reservedAmount = MAX_ENEMY + 1;

	ViewData* cubeThingVD = ViewDataFromJson("CubeThing");

	int p = 0;
	while(p<ARRAY_SIZE)
	{
		particleArr[p] = new Particle();
		//Copy the contents of cubeThingVD into each new particle
		particleArr[p]->loadViewData( cubeThingVD, true);
		p++;
	}

	user = new Actor();
	user->loadViewData(cubeThingVD, true);
	//We need to load a new pointer, here, sor we're reloading the json file
	user->loadChildViewData( ViewDataFromJson("CubeThing") );
	user->init(0,0,0,0,0,true,&particleArr,ARRAY_SIZE,reservedAmount,&arrayIndex);


	particleArr[0] = user;

	int i = 0;
	while( i < MAX_ENEMY )
	{
		particleArr[i + 1] = new Actor();
		particleArr[i + 1]->loadViewData( cubeThingVD, true);

		particleArr[i + 1]->setActor(user);
		if(i == 0)
		{
			float r = rand() % 360;
			particleArr[i + 1]->init(((cos((r * XM_PI) / 180))* 500 + 400),
														(300 - (sin((r * XM_PI) / 180))* 500),
														0,0,0,false,&particleArr,ARRAY_SIZE,reservedAmount,&arrayIndex);
		}
		particleArr[i + 1]->applyLocation();
		i++;
	}

	CameraClass camera;
	camera.SetPosition(XMFLOAT3(0,0,0));
	cameraView = camera.QuickLook(user->getViewData()->world.getTrans(),Y);

	XMFLOAT4X4 projXMF4 = view->GetProjectionMatrix();
	XMMATRIX view, proj, mult;

	view = XMLoadFloat4x4(&cameraView);
	proj = XMLoadFloat4x4(&projXMF4);

	mult = XMMatrixMultiply(proj, view);
	
	inverseViewProjection =  XMMatrixInverse( nullptr, mult);

	ShowCursor(false);
	pointer = ViewDataFromJson("MiniThing");


	
	initialized = true;
	return true;
}

void Engine::processMouseInput(POINT di)
{

	//user->processMouseInput(di.x - view->windowWidth/2, di.y - view->windowHeight/2);

	
	//Our 0,0 starts from the center of the screen. We dat cool.
	int x = di.x - view->windowWidth/2;
	int y = di.y - view->windowHeight/2;
	//					ray
	//					/ \
	// * ----------------<>------------------> *
	//mouseNear		  MouseX,Y,Z			mouseFar
	XMFLOAT3 mouseNear = XMFLOAT3(x, y,0.0f);
	XMFLOAT3 mouseFar = XMFLOAT3(x, y, 1.0f);
	XMVECTOR vecNear, vecFar, direction, ray, shortRay;

	vecNear = XMVector3TransformCoord( XMLoadFloat3(&mouseNear), inverseViewProjection);
	vecFar = XMVector3TransformCoord( XMLoadFloat3(&mouseFar), inverseViewProjection);
	

	ray = XMVectorSubtract(vecFar, vecNear);

	direction = XMVector2Normalize(ray);

	shortRay = vecNear + (direction);

	XMFLOAT3 finalVec;
	//XMStoreFloat3(&finalVec, ray);
	XMStoreFloat3(&finalVec, shortRay);
	//printf("Aaand our final mousex and mousey values are: %f, %f!\n", finalVec.x, finalVec.y);
	//Arbitrary numbers? Yes! We're the best.
	mouseX = finalVec.x * 0.48;
	mouseY = finalVec.y * 0.65;
	user->processMouseInput(finalVec.x * 0.48, finalVec.y * 0.65);

}

void Engine::processKeyboardInput(BYTE input[256])
{
	if(input[DIK_UP] & 0x80 || input[DIK_W] & 0x80)
	{
		if(!holdingUp)
			user->sendCommand("up pressed");
		holdingUp = true;
	}
	else
	{
		if(holdingUp)
		{
			user->sendCommand("up released");
			holdingUp = false;
		}
	}
	if( input[DIK_DOWN] & 0x80 || input[DIK_S] & 0x80)
	{
		if(!holdingDown)
			user->sendCommand("down pressed");
		holdingDown = true;
	}
	else
	{
		if(holdingDown)
		{
			user->sendCommand("down released");
			holdingDown = false;
		}
	}
	if( input[DIK_LEFT] & 0x80  || input[DIK_A] & 0x80)
	{
		if(!holdingLeft)
			user->sendCommand("left pressed");
		holdingLeft = true;
	}
	else
	{
		if(holdingLeft)
		{
			user->sendCommand("left released");
			holdingLeft = false;
		}
	}
	if(input[DIK_RIGHT] & 0x80 || input[DIK_D] & 0x80)
	{
		if(!holdingRight)
			user->sendCommand("right pressed");
		holdingRight = true;
	}
	else
	{
		if(holdingRight)
		{
			user->sendCommand("right released");
			holdingRight = false;
		}
	}
	if(input[DIK_SPACE] & 0x80 || input[DIK_Q] & 0x80)
	{
		if(!holdingAttack)
			user->sendCommand("attack pressed");
		holdingAttack = true;
		//cameraLock = !cameraLock;
	}
	else
	{
		if(holdingAttack)
		{
			user->sendCommand("attack released");
			holdingAttack = false;
		}
	}
}