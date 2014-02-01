#include "Quaternion.h"

Quaternion::Quaternion()
{
	w = 1.0;
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

Quaternion::Quaternion(float angle, XMFLOAT3 axis)
{
	//cosf(angle/2), vx * sinf(angle/2), vy * sinf(angle/2), vz * sinf(angle/2)
	w = cosf(angle/2);
	x = axis.x * sinf(angle/2);
	y = axis.y * sinf(angle/2);
	z = axis.z * sinf(angle/2);
}

Quaternion::Quaternion(float roll, float pitch, float yaw)
{


}

XMFLOAT4X4* Quaternion::quatToMatrix()
{
	XMFLOAT4X4 qM;

	
	for(int j = 0; j < 4; j++)
		for(int i = 0; i < 4; i++)
			qM(j,i) = 0.0;

	//Diagonal
	qM(0,0) = 1.0f - 2.0f* pow(y,2.0f) - 2.0f * pow(z,2.0f);
	qM(1,1) = 1.0f - 2.0f* pow(x,2.0f) - 2.0f * pow(z,2.0f);
	qM(2,2) = 1.0f - 2.0f* pow(x,2.0f) - 2.0f * pow(y,2.0f);

	//Rest of top row
	qM(0,1) = 2.0f*x*y - 2.0f*w*z;
	qM(0,2) = 2.0f*x*z + 2.0f*w*y;

	//Rest of middle row
	qM(1,1) = 2.0f*x*y + 2.0f*w*z;
	qM(1,2) = 2*y*z - 2*w*x;

	//Rest of bottom row
	qM(2,1) = 2.0f*x*z - 2.0f*w*y;
	qM(2,2) = 2.0f*y*z + 2.0f*w*x;

	//Our translation (for the sake of this program)
	//qM( = -5.0f;
	//qM[15] = 1.0f;
	//printMatrix(matrix);

	return &qM;

}

Quaternion Quaternion::operator= (Quaternion& rkQ)
{
	w = rkQ.w;
	x = rkQ.x;
	y = rkQ.y;
	z = rkQ.z;

	return *this;
}

Quaternion Quaternion::operator* (Quaternion& rkQ) const
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.

	printf("Multiplying ");
	toString();
	printf(" with ");
	rkQ.toString();

    return Quaternion
    (
        w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z,
        w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y,
        w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z,
        w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x
    );
}

Quaternion Quaternion::operator* (float fScalar) const
{
    return Quaternion(fScalar*w,fScalar*x,fScalar*y,fScalar*z);
}

Quaternion Quaternion::operator*= (Quaternion& rkQ)
{
    // NOTE:  Multiplication is not generally commutative, so in most
    // cases p*q != q*p.
	Quaternion *newQ = new Quaternion();

    newQ->w = w * rkQ.w - x * rkQ.x - y * rkQ.y - z * rkQ.z;
    newQ->x = w * rkQ.x + x * rkQ.w + y * rkQ.z - z * rkQ.y;
    newQ->y = w * rkQ.y + y * rkQ.w + z * rkQ.x - x * rkQ.z;
    newQ->z = w * rkQ.z + z * rkQ.w + x * rkQ.y - y * rkQ.x;
    
	w = newQ->w;
	x = newQ->x;
	y = newQ->y;
	z = newQ->z;

	return *this;
}


float Quaternion::norm() const
{
	return w*w+x*x+y*y+z*z;
}

void Quaternion::normalize()
{
	float len = norm();
	float factor = 1.0f / sqrt(len);
	*this = *this * factor;
}

float* Quaternion::xAxis() const
{
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxy = fTy*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;
		float fTzz = fTz*z;

		float vector[3] = {1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy};

        return &vector[0];
}

float* Quaternion::yAxis() const
{
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwx = fTx*w;
		float fTwz = fTz*w;
		float fTxx = fTx*x;
		float fTxy = fTy*x;
		float fTyz = fTz*y;
		float fTzz = fTz*z;

		float vector[3] = {fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx};

		return &vector[0];

}

float* Quaternion::zAxis() const
{
	float fTx  = 2.0f*x;
	float fTy  = 2.0f*y;
	float fTz  = 2.0f*z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTxx = fTx*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;

	float vector[3] = {fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy)};

	return &vector[0];
}

void Quaternion::toString() const
{
	
	("W: %f, X: %f, Y: %f, Z: %f\n", w, x, y, z);
}

Quaternion::~Quaternion(void)
{
}
