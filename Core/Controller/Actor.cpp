	#pragma once
#include "Actor.h"

/*
	Velocity in this game is determined by the pieces that you have attached:
	Each piece added to you will weight you down, in this case by .04. There
	are "speed" pieces that will increment you overall speed by .24. The total
	speed is determined by taking all pieces into account, and while there is
	no velocity ceiling there is a floor set at .08.
	*/
	static const float BASE_VELOCITY_ADD = .24;	//how much each speed piece adds to your velocity
	static const float BASE_PIECE_WEIGHT = .04;	//how much each non-speed piece slows you down
	static const float LOWEST_MAX_VELOCITY = .08;	//the lowest possible speed

	/*
	There is a cap of pieces that a given actor can hold
	*/
	/*
	a user can have a maximum of 200 pieces
									100 normal pieces
									50 attack pieces
									50 speed pieces
	*/
	static const float MAX_USER_PIECES = 200;
	static const float MAX_USER_NORMAL = 100;
	static const float MAX_USER_ATTACK = 50;
	static const float MAX_USER_SPEED = 50;
	/*
	an enemy can have a maximum of 75 pieces
									50 normal pieces
									15 attack pieces
									10 speed pieces
	*/
	static const float MAX_ENEMY_PIECES = 75;
	static const float MAX_ENEMY_NORMAL = 50;
	static const float MAX_ENEMY_ATTACK = 15;
	static const float MAX_ENEMY_SPEED = 10;

	/*
	as per request the actor's passive speed can be influenced by your movement,
	you, as the player, can influence your rotation by manually rotation with your
	movements, that being said pressing up right, down, left multiple times would
	cause you to rotate clockwise
	*/
	static const float MAX_ROTATION_RATE = 4;	//maximum rotation rate
	static const float MIN_ROTATION_RATE = -4;	//minimum rotation rate


Actor::Actor(void) : Particle()
{
	//all set to default
	countNormal = 0;
	countSpeed = 0;
	countAttack = 0;
	countTotal = 0;

	arrowDown = false;
	arrowLeft = false;
	arrowRight = false;
	arrowUp = false;
	attackButton = false;
	rotationRate = 0;
	rotationInfluenceMagnitude = 0.8;
	previousDirection = 0;
	rotation = 0;
	counter = 0;

	mouseX = 0;
	mouseY = 0;

	enemyMovementAngle = 0;
	fireCooldown = 45;

	hasParticleViewData = false;

}
Actor::~Actor(void)
{
	//needs to be filled
}
/*Initiates the actor to player settings and numbers and is called by init in
accordance to the type of actor requested*/
void Actor::playerInit()
{
	//sets the maximums to user maximums
	maxNormal = MAX_USER_NORMAL;
	maxSpeed = MAX_USER_SPEED;
	maxAttack = MAX_USER_ATTACK;
	//generate particles attached to the actor
	generateParticles();
}
/*Initiates the actor to enemy settings and numbers and is called by init in
accordance to the type of actor requested*/
void Actor::enemyInit()
{
	//sets the off-angle to a random number
	if(rand() % 2 == 0)
	{
		enemyMovementAngle = fixAngle((int)(rand() % 21) + 60);
	}
	else
	{
		enemyMovementAngle = fixAngle(-1 * ((int)(rand() % 21) + 60));
	}
	//sets the maximums to enemy maximums
	maxNormal = MAX_ENEMY_NORMAL;
	maxSpeed = MAX_ENEMY_SPEED;
	maxAttack = MAX_ENEMY_ATTACK;
	

	//sets the direction at which the enemy is moving to a random angle
	angleDirection = rand() % 360;
	//generates particles attached to the actor
	generateParticles();

	angleAcceleration = 1000; // don't worry it gets reassigned
}

void Actor::init(float ilocationX,float ilocationY,float iangleDirection,float iangleVelocity,float iangleAcceleration,bool isPlayer, vector<Particle*> *iparticleArr, int iparticleArrSize, int ireservedAmount,int *iarrayIndex)
{
	//counters set to 0
	countNormal = 0;
	countSpeed = 0;
	countAttack = 0;
	countTotal = 0;
	//it is now active and being drawn
	active = true;
	drawing = true;
	//pointer to the engine assigned
	particleArr = iparticleArr;
	particleArrSize = iparticleArrSize;
	reservedAmount = ireservedAmount;
	arrayIndex = iarrayIndex;
	if(isPlayer)
	{
		maxTotal = MAX_USER_PIECES;
	}
	else
	{
		maxTotal = MAX_ENEMY_PIECES;
	}
	arrPieces = vector<Particle*>(maxTotal);
	//if it is the player initialize it as one
	if(isPlayer)
	{
		initParticle(ilocationX,ilocationY,iangleDirection,iangleVelocity,iangleAcceleration,core,player,NULL);
		playerInit();
	}
	else
	{
		//otherwise this
		initParticle(ilocationX,ilocationY,iangleDirection,iangleVelocity,iangleAcceleration,core,enemy,NULL);
		enemyInit();
	}
}
/* calls on enemyAct() and playerAct() accordingly. updates acceleration,
velocity, location, and rotation. increments counter for fireCooldown until
it reaches the cooldown and is set to 0*/
void Actor::act(float magnitude)
{
	if(relationship == player)
	{
		//if this is the player then act as one
		playerAct(magnitude);
	}
	else if(relationship == enemy)
	{
		//if this is an enemy then act as one
		enemyAct(magnitude);
	}
	//update these and apply them
	applyAcceleration(magnitude);
	applyVelocity(magnitude);
	applyLocation();
	applyRotation(magnitude);

	if(counter > 0)
	{
		/*
		cooldown for shooting, if it at 0 you are capable of shooting, otherwise it is on
		cooldown.
		*/
		counter += magnitude;
		if(counter > fireCooldown)
		{
			counter = 0;
		}
	}
}
/* updates the target coordinates. sets angleDirection. and attacks if capable */
void Actor::enemyAct(float magnitude)
{
	//update target coordinates
	targetX = targetActor->drawX;
	targetY = targetActor->drawY;

	if(getDistance(drawX,drawY,targetX,targetY) > 400)
	{
		//if it is 400 away then lets get closer by moving directly at our target
		angleDirection = getAngle(drawX,drawY,targetX,targetY);
	}
	else
	{
		//get angle from this to target, apply enemyMovementAngle as well
		float n = fixAngle(angleDirection - getAngle(drawX,drawY,targetX,targetY) + enemyMovementAngle);

		if(n > 180)
		{
			//cw
			angleDirection = fixAngle(angleDirection + (10 * magnitude));
		}
		else if(n < 180)
		{
			//ccw
			angleDirection = fixAngle(angleDirection - (10 * magnitude));
		}

		if(counter == 0)
		{
			//attack if you can, and set it on cooldown
			useAttack();
			counter = 1;
		}
	}
}
/* updates the angleDirection, angleVelocity, rotation according to player input */
void Actor::playerAct(float magnitude)
{
	if((arrowLeft == arrowRight) && (arrowUp == arrowDown))
	{
		angleAcceleration=-.1;
		influenceRotation(magnitude);
	}
	else if(arrowLeft == arrowRight && arrowUp && !arrowDown)
	{
		//north
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 90;
		influenceRotation(magnitude);
	}
	else if(arrowLeft == arrowRight && !arrowUp && arrowDown)
	{
		//south
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 270;
		influenceRotation(magnitude);
	}
	else if(arrowLeft && !arrowRight && arrowUp == arrowDown)
	{
		//west
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 180;
		influenceRotation(magnitude);
	}
	else if(arrowLeft && !arrowRight && arrowUp && !arrowDown)
	{
		//north west
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 135;
		influenceRotation(magnitude);
	}
	else if(arrowLeft && !arrowRight && !arrowUp && arrowDown)
	{
		//south west
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 225;
		influenceRotation(magnitude);
	}
	else if(!arrowLeft && arrowRight && arrowUp == arrowDown)
	{
		//east
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 0;
		influenceRotation(magnitude);
	}
	else if(!arrowLeft && arrowRight && arrowUp && !arrowDown)
	{
		//north east
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 45;
		influenceRotation(magnitude);
	}
	else if(!arrowLeft && arrowRight && !arrowUp && arrowDown)
	{
		//south east
		angleAcceleration=.16;
		previousDirection = angleDirection;
		angleDirection = 315;
		influenceRotation(magnitude);
	}
		
	if(attackButton)
	{
		//do things
		if(counter == 0)//not on cooldown)
		{
			useAttack();
			counter = 1;//now on cooldown
		}
	}
}
/*generates a projectile from all attack pieces attached to the actor and
	assignes to them the coordinates that they must move towards*/
void Actor::useAttack()
{
	int i = 0;
	while(i < countTotal)//loop "countTotal"nth times
	{
		if(arrPieces[i]->type == attack)
		{
			//if the current particle we are looking at at the array is an
			//attack particle then lets try to attack from it
			Particle *temp = new Particle();
			if(hasViewData)
				temp->loadViewData(particleViewData, true);
			int reply = addToParticleArray(temp);
			if(reply != -1)
			{
				//initiate the particle
				temp->initParticle(arrPieces[i]->drawX,arrPieces[i]->drawY,
					getAngle(drawX, drawY,arrPieces[i]->drawX,arrPieces[i]->drawY),
					10,0,projectile,relationship,NULL);

				if(relationship == player)
				{
					/*                      DIRECTX HELP NEEDED HERE                         */
					/*
						the target for this bullet should be set to the mouse's current coordinates
						
						how would one go about obtaining the mouse's coordinates and pass them along
					*/
					temp->setTarget(mouseX/*mouse x coordinate*/,mouseY/*mouse x coordinate*/);
				}
				else
				{
					temp->setTarget(targetActor->drawX,targetActor->drawY);
				}
			}
			else
				printf("I HATE MY LIFE\n");
		}
		i++;
	}
}
/*assigns the rotation of the actor accoding to current rotation
	 and previous rotation*/
void Actor::influenceRotation(float magnitude)
{
	//check if it is the same direction
	if(previousDirection != angleDirection)
	{
		//get the difference between the two angles
		float n = fixAngle(previousDirection - angleDirection);

		if(n > 180)
		{
			//clockwise
			//incremenent angle (factoring in magnitude)
			rotationRate += rotationInfluenceMagnitude * magnitude;
			if(rotationRate > MAX_ROTATION_RATE)
						rotationRate = MAX_ROTATION_RATE;
		}
		if(n < 180)
		{
			//counterclockwise
			//decremenent angle (factoring in magnitude)
			rotationRate -= rotationInfluenceMagnitude * magnitude;
			if(rotationRate < MIN_ROTATION_RATE)
						rotationRate = MIN_ROTATION_RATE;
		}
	}
	else
	{
		//if it is the same lets slow down the rotation a bit and get to 0 slowly
		if(rotationRate > 0)
			rotationRate -= (rotationInfluenceMagnitude/20 * magnitude);
		else if(rotationRate < 0)
			rotationRate += (rotationInfluenceMagnitude/20 * magnitude);
	}
}
/*uses the newly refreshed values to rotate the actor and it's particles 
	accordingly*/
void Actor::applyRotation(float magnitude)
{
	int i = 0;
	while(i < countTotal)
	{
		//loop countTotal amount of times to change the location of all attached particles in the array
		if(arrPieces[i]->ownerActor == this)
		{
			//lets get the initial angle and distance from the core to the particle
			//get angle and increment by (rotationRate * magnitude)
			float angle = getAngle(0,0,arrPieces[i]->locationX,arrPieces[i]->locationY) + (rotationRate * magnitude);
			float distance = getDistance(0,0,arrPieces[i]->locationX,arrPieces[i]->locationY);
			//once we have that lets change the location by mathing the new x and y from our set
			//distance and new angle
			arrPieces[i]->locationX = getXinDirection(angle,distance,0);
			arrPieces[i]->locationY = getYinDirection(angle,distance,0);
			//update the location at which the particle is drawn
			arrPieces[i]->drawX = getXinDirection(angle,distance,drawX);
			arrPieces[i]->drawY = getYinDirection(angle,distance,drawY);
		}
		i++;
	}
}
/*
	when spawned, an actor generates particles and attaches them to the core
	*/
void Actor::generateParticles()
{
	applyLocation();

	int i = 0;
	int basePiece = 0;
	float angle = 0;
	float distance = 10;
	int numofParticles = 2;
	int randType = 0;//type of generated particle; default to normal

	if(relationship == player)
	{
		numofParticles = 2;
	}
	else if(relationship == enemy)
	{
		numofParticles = (rand()%(maxTotal - 9 ) + 10);
	}

	while(i < numofParticles)
	{
		Type t = normal;
		angle = rand() % 360;
		if(relationship == player)
		{
			if(i<4)
				randType = 2;
			else if(i<8)
				randType = 3;
			else
				randType = 0;
		}
		else if(relationship == enemy)
		{
			randType = rand() % 4;

			if(randType == 1)
				randType = 0;
		}

		if(randType == 2)
			t = attack;
		if(randType == 3)
			t = speed;
		if(randType == 0)
			t = normal;

		if(countTotal == 0)
			basePiece = 1;
		else
			basePiece = (int)(rand() % countTotal + 1);

		arrPieces[countTotal] = new Particle();
		
		if(hasViewData)
			arrPieces[countTotal]->loadViewData(particleViewData, true);

		int reply = addToParticleArray(arrPieces[countTotal]);

		if(reply != -1)//if it got accepted
		{
			//increment appropriately
			if(randType==attack)
				countAttack++;
			if(randType==speed)
				countSpeed++;
			if(randType==normal)
				countNormal++;
			/*
			this algorith determines the location of the generated particle by selecting a random
			piece (determined by basePiece)in the array or the core and then getting an x and y 
			from that location and an angle away from it 
			*/
			if(basePiece == (countTotal + 1))
			{
				//we are basing this location from the core
				arrPieces[countTotal]->initParticle(getXinDirection(angle,10,0),getYinDirection(angle,10,0),
					angle,0,-0.1,t,relationship,this);
			}
			else
			{
				//we are basing this location from a particle in the array
				arrPieces[countTotal]->initParticle(getXinDirection(angle,10,arrPieces[basePiece]->locationX),
					getYinDirection(angle,10,arrPieces[basePiece]->locationY),angle,0,-0.1,t,relationship,this);
			}
			arrPieces[countTotal]->applyLocation();//lets update this
			arrPieces[countTotal]->onAttachToCore(relationship,this,countTotal);//let it know it now belongs to an actor
			countTotal++;//increment count be
		}
		else
		{
			printf("we have failed\n");
		}

		i++;
	}
	assignMaximumVelocity();
}
/*adds the particle's pointer to the array of particle pointers to later
	assign their location based on the actor's location and rotation*/
bool Actor::addToArray(Particle *p)
{
	//while the count total is still below the max
	if(countTotal < maxTotal)
	{
		//get the angle and direction from the core to the particle we just picked up
		float angle = getAngle(drawX,drawY,p->drawX,p->drawY);
		float distance = getDistance(drawX,drawY,p->drawX,p->drawY);

		//change the location from "coordinate in game" to "location in relation to actor"
		p->locationX = getXinDirection(angle,distance,0);
		p->locationY = getYinDirection(angle,distance,0);
		p->angleDirection = angle;//angle from actor to particle will be stored in angleDirection of particle

		if(p->type == attack && countAttack < maxAttack)
		{
			arrPieces[countTotal] = p;
			p->onAttachToCore(relationship,this,countTotal);
			countAttack++;
			countTotal++;
			assignMaximumVelocity();
			return true;
		}
		else if(p->type == speed && countAttack < maxSpeed)
		{
			arrPieces[countTotal] = p;
			p->onAttachToCore(relationship,this,countTotal);
			countSpeed++;
			countTotal++;
			assignMaximumVelocity();
			return true;
		}
		else if(p->type == normal && countAttack < maxNormal)
		{
			arrPieces[countTotal] = p;
			p->onAttachToCore(relationship,this,countTotal);
			countNormal++;
			countTotal++;
			assignMaximumVelocity();
			return true;
		}
	}
	else
	{
		p->sendCommand("remove");
	}
	return false;
}
void Actor::removeFromArray(int particleIndex)
{
	if(particleIndex < countTotal)
	{
		arrPieces[particleIndex]->locationX = arrPieces[particleIndex]->drawX;
		arrPieces[particleIndex]->locationY = arrPieces[particleIndex]->drawY;
		arrPieces[particleIndex]->sendCommand("destroy");

		if(arrPieces[particleIndex]->type == normal)
		{
			countNormal--;
			assignMaximumVelocity();
		}
		else if(arrPieces[particleIndex]->type == attack)
		{
			countAttack--;
			assignMaximumVelocity();
		}
		else if(arrPieces[particleIndex]->type == speed)
		{
			countSpeed--;
			assignMaximumVelocity();
		}

		int i = particleIndex;
		while(i < countTotal)
		{
			if(i == countTotal -1)
			{
				/*
				end or array
				arrPieces[i] = NULL;
				*/
			}
			else
			{
				arrPieces[i] = arrPieces[i + 1];
				arrPieces[i]->index = i;
			}
			i++;
		}
		countTotal--;
	}
}
void Actor::assignMaximumVelocity()
{
	maxVelocity = (3) - (countTotal * BASE_PIECE_WEIGHT) + (countSpeed * BASE_VELOCITY_ADD);
	if(maxVelocity < LOWEST_MAX_VELOCITY)
	{
		maxVelocity = LOWEST_MAX_VELOCITY;
	}
}

void Actor::remove()
{
	active = false;
	int i = 0;
	while(i < countTotal)
	{
		arrPieces[i]->sendCommand("destroy");
		i++;
	}
}
void Actor::sendCommand(string command)
{
	if(command == "up pressed")
	{
		arrowUp = true;
	}
	else if(command == "down pressed")
	{
		arrowDown = true;
	}
	else if(command == "left pressed")
	{
		arrowLeft = true;
	}
	else if(command == "right pressed")
	{
		arrowRight = true;
	}
	else if(command == "attack pressed")
	{
		attackButton = true;
	}
	else if(command == "up released")
	{
		arrowUp = false;
	}
	else if(command == "down released")
	{
		arrowDown = false;
	}
	else if(command == "left released")
	{
	arrowLeft = false;
	}
	else if(command == "right released")
	{
		arrowRight = false;
	}
	else if(command == "attack released")
	{
		attackButton = false;
	}
	else if(command == "destroy")
	{
		remove();
	}
	else
	{
		Particle::sendCommand(command);	/*someone double check if it is done like this. in other languages it*/
										/*is handled with a call like "super.sendCommand(command)"			 */
	}
}
void Actor::setActor(Particle *itargetActor)
{
	targetActor = itargetActor;
}

bool Actor::checkAllCollision(float px, float py, float radius1, float radius2, Relationship iRelationship)
{
	if(relationship == iRelationship)
	{
		return false;
	}

	int i = 0;

	while(i < countTotal)
	{
		if(relationship == neutral)
		{
			if(checkCollision(px,py,arrPieces[i]->drawX,arrPieces[i]->drawY,radius1,radius2))
			{
				return true;
			}
		}
		else if(relationship != iRelationship)
		{
			if(checkCollision(px,py,arrPieces[i]->drawX,arrPieces[i]->drawY,radius1,radius2))
			{
				removeFromArray(arrPieces[i]->index);
				return true;
			}
		}
		i++;
	}

	if(iRelationship == normal)
	{
		return (checkCollision(px,py,drawX,drawY,radius1,radius2));
	}
	else if(relationship != iRelationship)
	{
		if(checkCollision(px,py,drawX,drawY,radius1,radius2))
		{
			sendCommand("destroy");
			return true;
		}
	}

	return false;
}
int Actor::addToParticleArray(Particle *p)
{
	if(*arrayIndex < particleArrSize)
	{
		int i = findNextUnactive();
		if(i == -1)
		{
			return -1;
		}
		else
		{
			p->drawing = true; // just as a precaution
			(*particleArr)[i] = p;
			(*arrayIndex)++;
		}
		return i;
	}
	else
	{
		return -1;
	}
}
int Actor::findNextUnactive()
{
	int i = reservedAmount;
	while(i < particleArrSize)
	{
		if(!((*particleArr)[i]->active))
			return i;
		i++;
	}
	return -1;
}

void Actor::loadChildViewData(ViewData* vd)
{
	particleViewData = vd;
	hasParticleViewData = true;
}

void Actor::processMouseInput(int x, int y)
{
	mouseX = x;
	mouseY = y;
}