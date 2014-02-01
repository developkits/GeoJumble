////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Camera.h"


CameraClass::CameraClass()
{
	world = World(XMFLOAT3(), XMFLOAT3(), XMVECTOR());
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(XMFLOAT3 input)
{
	world.tX(input.x);
	world.tY(input.y);
	world.tZ(input.z);
	return;
}

void CameraClass::incrementRotation(float angle, Axis axis_input)
{
	XMFLOAT3 axis = GetAxis(axis_input);

	//Quaternion rotDiff = Quaternion(angle, axis);
	XMVECTOR rotDiff = XMQuaternionRotationAxis(XMLoadFloat3(&axis), angle);

	XMVECTOR rotation = world.getRot();
	rotation *= rotDiff;
	
	rotation = XMQuaternionNormalize(rotation);
	world.setRot(rotation);
	
}


void CameraClass::SetRotation(XMVECTOR input)
{
	world.setRot(input);
	return;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return world.getTrans();
}


XMVECTOR CameraClass::GetRotation()
{
	return world.getRot();
}

void CameraClass::Render()
{
	XMFLOAT3 _lookAt, _up, _position;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	_up.x = 0.0f;
	_up.y = 1.0f;
	_up.z = 0.0f;


	// Setup the position of the camera in the world.
	_position = world.getTrans();

	// Setup where the camera is looking by default.
	_lookAt.x = 0.0f;
	_lookAt.y = 0.0f;
	_lookAt.z = 0.0f;

	
	
	XMVECTOR lookAt = XMLoadFloat3(&_lookAt);
	XMVECTOR up = XMLoadFloat3(&_up);
	XMVECTOR pos = XMLoadFloat3(&_position);

	// Create the rotation matrix from the rotation quaternion.
	//rotationMatrix = XMMatrixRotationQuaternion( XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w) );
	rotationMatrix = XMMatrixRotationQuaternion( world.getRot() );


	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.	
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord( up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = pos + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(pos, lookAt,up);

	return;
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

//Will return a view matrix looking at a specific position based on the current position
XMFLOAT4X4 CameraClass::QuickLook(XMFLOAT3 lookAt, Axis axis)
{
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&view,XMMatrixLookAtLH(XMLoadFloat3(&world.getTrans()),XMLoadFloat3(&lookAt),XMLoadFloat3(&GetAxis(axis))));
	return view;
}

DirectX::XMFLOAT3 CameraClass::GetAxis( Axis a )
{
	XMFLOAT3 axis;
	switch(a)
	{
	case X:
		axis = XMFLOAT3(1,0,0);
		break;
	case Y:
		axis = XMFLOAT3(0,1,0);
		break;
	case Z:
		axis = XMFLOAT3(0,0,1);
		break;
	}
	return axis;
}