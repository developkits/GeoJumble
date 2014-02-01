#pragma once
#include <stdlib.h>
#include <math.h>
#include <cstdio>
#include <DirectXMath.h>

using namespace DirectX;

class Quaternion
{
public:
	Quaternion();
	Quaternion(float angle, XMFLOAT3 axis);
	Quaternion(float w, float x, float y, float z);
	Quaternion(float roll, float pitch, float yaw);
	~Quaternion(void);

	Quaternion operator= (Quaternion& rkQ);
	Quaternion operator*(Quaternion& rkQ) const;
	Quaternion operator* (float fScalar) const;
	Quaternion operator*=(Quaternion& rkQ);

	float* xAxis() const;
	float* yAxis() const;
	float* zAxis() const;

	float w,x,y,z;
	XMFLOAT4X4* quatToMatrix();
	float norm() const;
	void normalize();

	void toString() const;
};

