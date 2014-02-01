#pragma once
#include "Particle.h"
#include <math.h>


using namespace std;

class Actor : public Particle
{

public:
	
	/*
	on run time when the type of actor is assigned (player or enemy) these
	settings will be set accordingly utelizing the constant variables above
	*/
	unsigned int maxNormal;
	unsigned int maxSpeed;
	unsigned int maxAttack;
	unsigned int maxTotal;
	/*
	while inititally 0, these will keep track of the amount of pieces, and which
	type, the actor has attached.
	*/
	unsigned int countNormal;
	unsigned int countSpeed;
	unsigned int countAttack;
	unsigned int countTotal;
	/*
	an array of pointers to keep track and manage the particles attached to the
	actor. These need to be pointers due to the fact that while they do manage their
	location while they are attacked to the core, they do not actually own them, the
	engine of the game must do that instead
	*/
	vector<Particle*> arrPieces;
	vector<Particle*> *particleArr;//array from engine
	int particleArrSize;
	int reservedAmount;
	int *arrayIndex;
	/*
	boolean values to keep track of the player's button presses.
	*/
	bool arrowLeft;
	bool arrowRight;
	bool arrowUp;
	bool arrowDown;
	bool attackButton;
	/*
	pointer to the engine of the game, this, while not prefered, is somewhat necessary
	to request the engine to create particles to enable creating particles at spawn or
	when shooting particles.
	*/
	/*
	dynamic values to maintain the aforementioned rotation feature
	*/
	float rotationRate;
	float rotationInfluenceMagnitude;
	float previousDirection;
	float rotation;
	float counter;
	/*
	pointer to the actor that you are shooting at. This is for enemies to be capable
	of accessing the player's draw location to shoot at it. The player might also be 
	using this later as a last resort if we are incapable of using the mouse to main
	*/
	Particle *targetActor; 
	/*
	used in determining the enemy's movment. Instead of just moving the enemy directly
	at the user, there is a randomly assigned number that enables the enemy to move at
	the user but at an angle. Let's say that the user it at 90degrees from the enemy,
	instead of the enemy moving at 90degrees the enem will instead move at 80, or 100,
	or 94.52843 yada yada
	*/
	float enemyMovementAngle;
	/*
	cooldown for shooting, if it at 0 you are capable of shooting, otherwise it is on
	cooldown.
	*/
	float fireCooldown;
	/*
	constructor & destructor
	*/
	Actor(void);
	~Actor(void);


	/* initiates the actor as a whole, calls on playerInit() and enemyInit()
	accordingly*/
	void init(float ilocationX,float ilocationY,float iangleDirection,
				float iangleVelocity,float iangleAcceleration,bool isPlayer, 
				vector<Particle*> *iparticleArr, int iparticleArrSize, int ireservedAmount,int *arrayIndex); // *
	/*Initiates the actor to player settings and numbers and is called by init in
	accordance to the type of actor requested*/
	void playerInit();
	/*Initiates the actor to enemy settings and numbers and is called by init in
	accordance to the type of actor requested*/
	void enemyInit();

	/* calls on enemyAct() and playerAct() accordingly. updates acceleration,
	velocity, location, and rotation. increments counter for fireCooldown until
	it reaches the cooldown and is set to 0*/
	void act(float magnitude);
	/* updates the target coordinates. sets angleDirection. and attacks if capable */
	void enemyAct(float magnitude);
	/* updates the angleDirection, angleVelocity, rotation according to player input */
	void playerAct(float magnitude);
	/*generates a projectile from all attack pieces attached to the actor and
	assignes to them the coordinates that they must move towards*/
	void useAttack();//*

	/*assigns the rotation of the actor accoding to current rotation
	 and previous rotation*/
	void influenceRotation(float magnitude);
	/*uses the newly refreshed values to rotate the actor and it's particles 
	accordingly*/
	void applyRotation(float magnitude);
	/*
	when spawned, an actor generates particles and attaches them to the core
	*/
	void generateParticles();
	/*adds the particle's pointer to the array of particle pointers to later
	assign their location based on the actor's location and rotation*/
	bool addToArray(Particle *p);
	/*removes the particle from the array. takes in the index of the particle*/
	void removeFromArray(int particleIndex);
	/*
	determines the maximum possible velocity from the amount of speed particles
	and non-speed particles currently attached to the actor
	*/
	void assignMaximumVelocity();

	/*
	overriden version of remove that instead of just setting itself to inactive
	and undrawable also makes sure to send the command "destroy" to all particles
	currently attached to the actor. if any are they are unattached and broken
	off (works the same way if they were shot off
	*/
	void remove();//*
	/*
	used for outside objects to send commands to the actor. For example for the 
	engine to alert the actor that the player has pressed certain buttons
	*/
	void sendCommand(string command);
	/*
	sets the target actor accordingly. For enemies to recognize who to shoot
	at
	*/
	void setActor(Particle *itargetActor);
	/*
	checks if any particles attached (including the actor itself) collide with
	a particle
	*/
	bool checkAllCollision(float px, float py, float radius1, float radius2, Relationship iRelationship);//*

	//The viewdata which will be copied over to each created particle
	bool hasParticleViewData;
	ViewData* particleViewData;
	void loadChildViewData(ViewData* vd);
	
	int addToParticleArray(Particle *p);
	int findNextUnactive(); //It's INactive, you silly Mexican.

	float mouseX, mouseY;
	void processMouseInput(int x, int y);

};