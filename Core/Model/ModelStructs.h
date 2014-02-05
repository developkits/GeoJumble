#pragma once

#include "Windows.h"
#include "World.h"
#include <string>

using namespace std;

struct ViewData
{
	World world;
	string texture;
	string mesh;
	string vertexShader;
	string pixelShader;
};

struct ViewInput
{
	ViewData* objects;
	int numObjects;
	XMFLOAT4X4 view;
	//Camera?
	//Projection Matrix?
	//etc
};