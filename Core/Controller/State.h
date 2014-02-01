#pragma once
#include "View.h"
#include <dinput.h>
#include "Camera.h"
#include "ModelStructs.h"
#include "ViewStructs.h"
#include "JsonUtils.h"

class State
{
public:
	virtual bool init() =0;
	virtual void run(float deltaT) =0;
	virtual void processKeyboardInput(BYTE input[256]) =0;
	virtual void processMouseInput(POINT) =0;
	bool initialized;

	ViewData* ViewDataFromJson( string objectName )
	{
		JsonUtils reader;
		string filename = "Resources/Objects/" + objectName;
		JsonData data = reader.ReadJsonFromFile(filename);
		ViewData* toReturn = new ViewData;

		toReturn->mesh = data.GetValue("Mesh").asString();
		view->LoadMesh(toReturn->mesh,false);

		toReturn->vertexShader = data.GetValue("VertexShader").asString();
		view->LoadVertexShaders(&(toReturn->vertexShader),1);

		toReturn->pixelShader = data.GetValue("PixelShader").asString();
		view->LoadPixelShaders(&(toReturn->pixelShader),1);

		Json::Value txts = data.GetValue("Textures");
		string* textures = new string[txts.size()];
		for(int i = 0; i < txts.size(); i++)
		{
			textures[i] = txts[i].asString();
		}
		toReturn->texture = textures[0];
		view->LoadTextures(textures,txts.size(),false);

		float tX = data.GetValue("World.Translation.x").asFloat();
		float tY = data.GetValue("World.Translation.y").asFloat();
		float tZ = data.GetValue("World.Translation.z").asFloat();
		float sX = data.GetValue("World.Scale.x").asFloat();
		float sY = data.GetValue("World.Scale.y").asFloat();
		float sZ = data.GetValue("World.Scale.z").asFloat();
		float rX = data.GetValue("World.Rotation.x").asFloat();
		float rY = data.GetValue("World.Rotation.y").asFloat();
		float rZ = data.GetValue("World.Rotation.z").asFloat();

		World world(tX,tY,tZ,sX,sY,sZ,rX,rY,rZ);

		toReturn->world = world;

		return toReturn;
	}

	View* getView() const { return view; } 

protected:
	View* view;
};