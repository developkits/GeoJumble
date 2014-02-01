	#pragma once
#include <math.h>
#include <string>
#include "ViewStructs.h"
#include "ModelStructs.h"
#include <vector>

enum Color { standard, normals, green, cyan, orange, black, purple, red, yellow, blue};

using namespace std;

class Particle
{
public:

	ViewData* viewData;
	ViewData* getViewData() const;
	void loadViewData(ViewData* vd, bool copy);

	void syncDraw();

	bool hasViewData;

	enum Relationship { neutral, player, enemy };
	enum Type { normal, projectile, attack, speed, core };

	float maxVelocity;
	bool active;
	bool drawing;

	float drawX;
	float drawY;

	float locationX;
	float locationY;

	float angleDirection;
	float angleVelocity;
	float angleAcceleration;

	int colorPreference;

	Relationship relationship;
	Type type;

	//mesh & texture

	bool obtainable;
	Particle *ownerActor;

	float targetX;
	float targetY;
	unsigned int timer;
	int index;

	Particle(void);
	~Particle(void);

	void initParticle(float ilocationX, float ilocationY, float iangleDirection,
		float iangleVelocity, float iangleAcceleration, Type iType,
		Relationship iRelationship, Particle *iownerActor);

	void setTarget(float itargetX,float itargetY);
	void setIndex(int iindex);

	virtual void act(float magnitude);

	void onAttachToCore(Relationship iRelationship, Particle *iownerActor, int iindex);

	virtual void sendCommand(string command);

	virtual void remove();

	void applyLocation();
	void applyVelocity(float magnitude);
	void applyAcceleration(float magnitude);

	float fixAngle(float angle);

	void moveInDirection(float angle, float distance);

	float getXinDirection(float angle, float distance, float positionX);
	float getYinDirection(float angle, float distance, float positionY);

	float getDistance(float p1x, float p1y, float p2x, float p2y);
	float getAngle(float p1x, float p1y, float p2x, float p2y);

	bool checkCollision(float p1x, float p1y, float p2x, float p2y, float radius1, float radius2);


	virtual	void init(float ilocationX,float ilocationY,float iangleDirection,
				float iangleVelocity,float iangleAcceleration,bool isPlayer, 
				vector<Particle*> *iparticleArr, int iparticleArrSize, int ireservedAmount,int *arrayIndex){}

	virtual void setActor(Particle *itargetActor){}

	virtual bool checkAllCollision(float px, float py, float radius1, float radius2, Relationship iRelationship){return false;}
};