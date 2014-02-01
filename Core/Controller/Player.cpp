#include <stdlib.h>;
#include <string>;
#include "World.h";
#include <ViewStructs.h>;
#include "Windows.h";
#include "Player.h";
using namespace std;




struct ViewInput
{
	World world;
	string texture;
	string mesh;
	string vertex;
	string pixel;


};


void Player:: setHealth(float h)
{
	health = h;

}

float Player:: getHealth()
{
	return health;

}