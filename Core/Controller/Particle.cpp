	#pragma once
#include "Particle.h"
//#include <../um/debugapi.h>

	static const unsigned int BASE_MAX_VELOCITY = 3;
	static const unsigned int DROP_RATE = 75;//drop chance out of 100, in this case 75

Particle::Particle(void)
{
	maxVelocity = 0;
	active = false;
	drawing = false;

	drawX = 0;
	drawY = 0;

	locationX = 0;
	locationY = 0;

	angleDirection = 0;
	angleVelocity = 0;
	angleAcceleration = 0;

	relationship = neutral;
	type = normal;

	obtainable = false;

	ownerActor = NULL;

	targetX = 0;
	targetY = 0;
	timer = 0;
	index = -1;

	colorPreference = 0;

	hasViewData = false;
}

Particle::~Particle(void)
{
	
}
void Particle::initParticle(float ilocationX, float ilocationY, float iangleDirection,
		float iangleVelocity, float iangleAcceleration, Type iType,
		Relationship iRelationship, Particle *iownerActor)
{
	maxVelocity = BASE_MAX_VELOCITY;
	active = true;
	drawing = true;

	locationX = ilocationX;
	locationY = ilocationY;

	drawX = locationX;
	drawY = locationY;

	angleDirection = iangleDirection;
	angleVelocity = iangleVelocity;
	angleAcceleration = iangleAcceleration;

	relationship = iRelationship;
	type = iType;

	obtainable = false;

	ownerActor = iownerActor;

	targetX = 0;
	targetY = 0;
	timer = 0;
	index = -1;

	if(relationship == normal)
	{
		/*    DIRECTX NEEDED HERE    */

		
		/*
			sets the piece into a spare/unobtainable piece
			in flash version:
					black
		*/

		//spare particle
		colorPreference = black;
		obtainable = false;
		maxVelocity = angleVelocity;
		angleAcceleration = -.1;
	}
	else if(relationship == player)
	{
		if(type == projectile)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user projectile
				in flash version:
					cyan circle
			*/
			//user projectile
			colorPreference = cyan;
	
		}
		else if(type == attack)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user attack piece/particle (the pieces that shoot)
				in flash version:
						red; perhaps a pyramid in this one
			*/
			//user attack particle (particle that shoot)
			colorPreference = red;
		}
		else if(type == speed)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user speed piece/particle
				in flash version:
						blue; perhaps a torus in this one
			*/
			colorPreference = blue;
			//user speed particle (particle that speeds up user)
		}
		else if(type == core)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user core
				in flash version:
						yellow; perhaps a sphereSP in this one
			*/
			//user core particle
			colorPreference = yellow;
		}
		else
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user normal piece/particle (the pieces that add weight)
				in flash version:
						green; perhaps a square in this one
			*/
			colorPreference = green;
			//user normal particle (doesn't bring anything but
			//weight and in the user's case brings the user closer
			//to the goal)
		}
	}
	else if(relationship == enemy)
	{
		if(type == projectile)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy projectile
				in flash version:
					orange circle
			*/
			//enemy projectile
			colorPreference = orange;
	
		}
		else if(type == attack)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy attack piece/particle (the pieces that shoot)
				in flash version:
						red; perhaps a pyramid in this one
			*/
			colorPreference = red;
			//enemy attack particle (particle that shoot)
		}
		else if(type == speed)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy speed piece/particle
				in flash version:
						blue; perhaps a torus in this one
			*/
			colorPreference = blue;
			//enemy speed particle (particle that speeds up user)
		}
		else if(type == core)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy core
				in flash version:
						yellow; perhaps a sphereSP in this one
			*/
			colorPreference = yellow;
			//enemy core particle
		}
		else
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy normal piece/particle (the pieces that add weight)
				in flash version:
						purple; perhaps a square in this one
			*/
			colorPreference = purple;

			//enemy normal particle (doesn't bring anything but
			//weight and in the user's case brings the user closer
			//to the goal)
		}
	}
	if(hasViewData)
		viewData->colorPreference = colorPreference;
}

void Particle::setTarget(float itargetX,float itargetY)
{
	targetX = itargetX;
	targetY = itargetY;
}

void Particle::setIndex(int iindex)
{
	index = iindex;
}

void Particle::act(float magnitude)
{
	//the logic for a spare flying particle is as follows:
	//when a peice is broken off an enemy or user it has a
	//chance of floating off as opposed of being deleted completely,
	//when this happens the spare particle is given velocity,
	//direction, and location. Once the speed reaches 0 it is then
	//capable of being picked up by the user and added to the user
	if(active && ownerActor == NULL)
	{
		timer+=magnitude;
		if(relationship != normal)
		{
			float n = fixAngle(angleDirection - getAngle(drawX,drawY,targetX,targetY));
			
			if(n >  180)
			{
				//cw
				angleDirection = fixAngle(angleDirection + (3 * magnitude));
			}
			if(n <  180)
			{
				//cw
				angleDirection = fixAngle(angleDirection - (3 * magnitude));
			}
		}
		if(angleVelocity != 0 )
		{
			applyAcceleration(magnitude);
			applyVelocity(magnitude);
			applyLocation();
		}
		else if(!obtainable)
		{
			/*    DIRECTX NEEDED HERE    */
	
			/*
				changes to grey
				mesh should probably be the same
			*/
		}
	}
}

void Particle::syncDraw()
{
	//printf("drawX: %f, drawY: %f\n", drawX, drawY);
	this->viewData->world.tX(drawX);
	this->viewData->world.tY(-drawY);
	//this->viewData->world.tZ(drawX);
	//printf("world.tX: %f, world.tY: %f, world.tZ: %f\n", viewData->world.getTrans().x, viewData->world.getTrans().y, viewData->world.getTrans().z);
}

void Particle::onAttachToCore(Relationship iRelationship, Particle *iownerActor, int iindex)
{
	ownerActor = iownerActor;
	index = iindex;

	if(iRelationship == player)
	{
		//apply actual colors
		if(type == projectile)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user projectile
				in flash version:
					cyan circle
			*/
			//user projectile
	
		}
		else if(type == attack)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user attack piece/particle (the pieces that shoot)
				in flash version:
						red; perhaps a pyramid in this one
			*/
			//user attack particle (particle that shoot)
		}
		else if(type == speed)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user speed piece/particle
				in flash version:
						blue; perhaps a torus in this one
			*/
			//user speed particle (particle that speeds up user)
		}
		else if(type == core)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user core
				in flash version:
						yellow; perhaps a sphereSP in this one
			*/
			//user core particle
		}
		else
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a user normal piece/particle (the pieces that add weight)
				in flash version:
						green; perhaps a square in this one
			*/

			//user normal particle (doesn't bring anything but
			//weight and in the user's case brings the user closer
			//to the goal)
		}
	}
	else if(iRelationship == enemy)
	{
		//apply actual colors
		if(type == projectile)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy projectile
				in flash version:
					cyan circle
			*/
			//enemy projectile
	
		}
		else if(type == attack)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy attack piece/particle (the pieces that shoot)
				in flash version:
						red; perhaps a pyramid in this one
			*/
			//enemy attack particle (particle that shoot)
		}
		else if(type == speed)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy speed piece/particle
				in flash version:
						blue; perhaps a torus in this one
			*/
			//enemy speed particle (particle that speeds up user)
		}
		else if(type == core)
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy core
				in flash version:
						yellow; perhaps a sphereSP in this one
			*/
			//enemy core particle
		}
		else
		{
			/*    DIRECTX NEEDED HERE    */

		
			/*
				sets the piece into a enemy normal piece/particle (the pieces that add weight)
				in flash version:
						green; perhaps a square in this one
			*/

			//enemy normal particle (doesn't bring anything but
			//weight and in the user's case brings the user closer
			//to the goal)
		}
	}
}

ViewData* Particle::getViewData() const
{
	if(hasViewData)
		return this->viewData;
	return 0;
}

void Particle::loadViewData(ViewData* vd, bool copy)
{
	if(copy)
		viewData = new ViewData(*vd);
	else
		viewData = vd;
	hasViewData = true;

	if(colorPreference)
		viewData->colorPreference = colorPreference;
}


void Particle::sendCommand(string command)
{
	ownerActor = NULL;
	locationX = drawX;
	locationY = drawY;

	if(command == "remove")
	{
		if((rand() % 101)>= DROP_RATE)
		{
			float randTheta = rand() % 360;
			if(type == core)
				type = normal;

			initParticle(drawX,drawY,randTheta, 5, -0.1, type, neutral , NULL);
			timer = 0;
		}
		else
		{
			remove();
		}
	}
}
void Particle::remove()
{
	active = false;
}
void Particle::applyLocation()
{
	if(ownerActor == NULL)
	{
		drawX = locationX;
		drawY = locationY;
	}
}

void Particle::applyVelocity(float magnitude)
{
	moveInDirection(angleDirection,(angleVelocity * magnitude));
}

void Particle::applyAcceleration(float magnitude)
{
	angleVelocity += (angleAcceleration * magnitude);
	if(angleVelocity < 0)
		angleVelocity = 0;
	else if(angleVelocity > maxVelocity)
		angleVelocity = maxVelocity;
}

float Particle::fixAngle(float angle)
{
	while(angle<0 || angle>=360)
	{
		if(angle<0)
			angle += 360;
		else
			angle -= 360;
	}
	return angle;
}

void Particle::moveInDirection(float angle, float distance)
{
	angle = fixAngle(angle);
	locationX = ((cos(((angle * XM_PI) / 180)) * distance) + locationX);
	locationY = (locationY - (sin(((angle * XM_PI) / 180)) * distance));
}

float Particle::getXinDirection(float angle, float distance, float positionX)
{
	angle = fixAngle(angle);
	return ((cos((angle * XM_PI) / 180))* distance + positionX);
}

float Particle::getYinDirection(float angle, float distance, float positionY)
{
	angle = fixAngle(angle);
	return (positionY - (sin((angle * XM_PI) / 180))* distance);
}

float Particle::getDistance(float p1x, float p1y, float p2x, float p2y)
{
	return sqrt(((p2x - p1x) * (p2x - p1x)) + ((p2y - p1y) * (p2y - p1y)));
}

float Particle:: getAngle(float p1x, float p1y, float p2x, float p2y)
{
	if (p2x == p1x && p2y == p1y)
		return 0;
    if (p2x < p1x)
        return 180 - (((atan((p2y - p1y) / (p2x - p1x))) * (180 / XM_PI)));
    return 360 - (((atan((p2y - p1y) / (p2x - p1x))) * (180 / XM_PI)));
}

bool Particle::checkCollision(float p1x, float p1y, float p2x, float p2y, float radius1, float radius2)
{
	return((getDistance(p1x,p1y,p2x,p2y))<(radius1 + radius2));
}