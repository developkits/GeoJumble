#pragma once

#include <DirectXMath.h>
#include "Quaternion.h"

using namespace DirectX;
class World
{
public:
	World(float tx, float ty, float tz,
			float sx, float sy, float sz,
			float rx, float ry, float rz);
	World(XMFLOAT3 t, XMFLOAT3 s, XMVECTOR r);
	World(void);
	~World(void);
	XMFLOAT4X4 operator () ();
	void tX(float i);
	void tY(float i);
	void tZ(float i);
	void sX(float i);
	void sY(float i);
	void sZ(float i);

	void setTrans(XMFLOAT3 t);
	void setScale(XMFLOAT3 s);
	void setRot(XMVECTOR r);

	void rotate(XMVECTOR axis, float angle);
	void rotate(XMVECTOR toRotate);

	XMFLOAT3 getTrans();
	XMFLOAT3 getScale();
	XMVECTOR getRot();

private:
	XMFLOAT3 translation;
	XMFLOAT3 scale;
	XMVECTOR rotation;

	bool dirty;

	XMFLOAT4X4 world;
};

