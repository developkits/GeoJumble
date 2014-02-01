#include <stdlib.h>;
#include <string>;
#include "World.h";
#include <ViewStructs.h>;
#include "Windows.h";
#include "Enemy.h";
using namespace std;




struct ViewInput
{
	World world;
	string texture;
	string mesh;
	string vertex;
	string pixel;


};


void Enemy:: setHealth(float h)
{
	health = h;

}

float Enemy:: getHealth()
{
	return health;

}