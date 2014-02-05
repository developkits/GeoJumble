#pragma once
#include "ModelStructs.h"
#include <string.h>
#include "View.h"
#include "JsonUtils.h"

//Starting point for dealing with ONLY gameplay code
class Engine
{
public:
	Engine(void);
	~Engine(void);
	ViewInput Update(float deltaTime);
};

