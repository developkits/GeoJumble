#include "World.h"

World::World( float tx, float ty, float tz, float sx, float sy, float sz, float roll, float pitch, float yaw )
{
	translation = XMFLOAT3(tx, ty, tz);
	scale = XMFLOAT3(sx,sy,sz);
	rotation = XMQuaternionRotationRollPitchYaw(roll, pitch, yaw);
	//rotation = Quaternion(1.0f,rx,ry,rz);
	dirty = true;
}

World::World( XMFLOAT3 t, XMFLOAT3 s, XMVECTOR r )
{
	translation = t;
	scale = s;
	rotation = r;
	//No args = unit q
	//rotation = Quaternion(1.0f,r.x,r.y,r.z);
	dirty = true;
}

World::World(void)
{
	dirty = true;
}

World::~World(void)
{
}

DirectX::XMFLOAT4X4 World::operator()()
{
	if(dirty)
	{
		//This will be what we return
		//Start with an identity matrix
		XMMATRIX  transMat = XMMatrixTranslation(translation.x,translation.y,translation.z);
		XMMATRIX scaleMat = XMMatrixScaling(scale.x,scale.y,scale.z);
		XMMATRIX rotMat = XMMatrixRotationQuaternion(rotation);
		//XMMATRIX rotMat = XMLoadFloat4x4(rotation.quatToMatrix());

		//Multiply together
		XMMATRIX worldMat = XMMatrixMultiply(XMMatrixMultiply(scaleMat,rotMat),transMat);
		//Place the matrix into the world!
		XMStoreFloat4x4(&world,worldMat);

		dirty = false;
	}
	return world;
}

void World::tX( float i )
{
	translation.x = i;
	dirty = true;
}

void World::tY( float i )
{
	translation.y = i;
	dirty = true;
}

void World::tZ( float i )
{
	translation.z = i;
	dirty = true;
}

void World::sX( float i )
{
	scale.x = i;
	dirty = true;
}

void World::sY( float i )
{
	scale.y = i;
	dirty = true;
}

void World::sZ( float i )
{
	scale.z = i;
	dirty = true;
}


void World::setTrans( XMFLOAT3 t )
{
	translation = t;
	dirty = true;
}

void World::setScale( XMFLOAT3 s )
{
	scale = s;
	dirty = true;
}

void World::setRot( XMVECTOR r )
{
	rotation = r;
	dirty = true;
}

DirectX::XMFLOAT3 World::getTrans()
{
	return translation;
}

DirectX::XMFLOAT3 World::getScale()
{
	return scale;
}

XMVECTOR World::getRot()
{
	return rotation;
}

XMMATRIX World::getMat()
{
	return XMLoadFloat4x4(&world);
}

void World::rotate(XMVECTOR axis, float angle)
{
	XMVECTOR toRotate = XMQuaternionRotationAxis(axis, angle);
	rotation = XMQuaternionNormalize(XMQuaternionMultiply(toRotate, rotation));
	dirty = true;
}

void World::rotate(XMVECTOR toRotate)
{
	rotation = XMQuaternionMultiply(toRotate, rotation);
	dirty = true;
}
