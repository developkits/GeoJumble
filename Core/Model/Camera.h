////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
#include "Quaternion.h"
#include "World.h"
#include <math.h>

using namespace DirectX;
////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////

enum Axis
{
	X,Y,Z
};

class CameraClass
{
public:


	XMFLOAT3 GetAxis(Axis a);
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(XMFLOAT3 input);
	void SetRotation(XMVECTOR input);
	void incrementRotation(float angle, Axis axis_input);

	XMFLOAT3 GetPosition();
	XMVECTOR GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);
	XMFLOAT4X4 QuickLook(XMFLOAT3 lookAt, Axis axis);

private:
	XMMATRIX m_viewMatrix;
	World world;
};

#endif