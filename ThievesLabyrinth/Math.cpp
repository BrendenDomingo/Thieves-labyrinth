#include "Math.h"
#include <math.h>
#include <string>

CMath::TVECTOR3::TVECTOR3()
{
	x = y = z = 0;
}

CMath::TVECTOR3::TVECTOR3(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator+(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x + V.x;
	result.y = y + V.y;
	result.z = z + V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator-(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x - V.x;
	result.y = y - V.y;
	result.z = z - V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator-()
{
	TVECTOR3 tResult = *this;

	return tResult * -1.0f;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator+=(const TVECTOR3 V)
{
	x += V.x;
	y += V.y;
	z += V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator-=(const TVECTOR3 V)
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator*=(const TVECTOR3 V)
{
	x *= V.x;
	y *= V.y;
	z *= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator/=(const TVECTOR3 V)
{
	x /= V.x;
	y /= V.y;
	z /= V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator*=(float S)
{
	x *= S;
	y *= S;
	z *= S;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator/=(float S)
{
	x /= S;
	y /= S;
	z /= S;
	return *this;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator*(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x * V.x;
	result.y = y * V.y;
	result.z = z * V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator/(const TVECTOR3 V)
{
	TVECTOR3 result;
	result.x = x / V.x;
	result.y = y / V.y;
	result.z = z / V.z;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator*(float S)
{
	TVECTOR3 result;
	result.x = x * S;
	result.y = y * S;
	result.z = z * S;
	return result;
}

CMath::TVECTOR3 CMath::TVECTOR3::operator/(float S)
{
	TVECTOR3 result;
	result.x = x / S;
	result.y = y / S;
	result.z = z / S;
	return result;
}

CMath::TVECTOR3& CMath::TVECTOR3::operator=(TVECTOR3 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	return *this;
}

CMath::TVECTOR3 & CMath::TVECTOR3::operator=(TVECTOR4 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	return *this;
}

bool CMath::TVECTOR3::operator==(TVECTOR3 V) const
{
	if (IsEqual(x, V.x) &&
		IsEqual(y, V.y) &&
		IsEqual(z, V.z))
	{
		return true;
	}
	return false;
}

bool CMath::TVECTOR3::operator!=(TVECTOR3 V) const
{
	return !( *this == V );
} 

bool CMath::IsEqual(float fA, float fB)
{
	return fabsf(fA - fB) < EPSILON;
}

bool CMath::IsEqual(TVECTOR2 V1, TVECTOR2 V2)
{
	if (IsEqual(V1.x,V2.x) &&
		IsEqual(V1.y, V2.y))
	{
		return true;
	}
	return false;
}

bool CMath::IsEqual(TVECTOR3 V1, TVECTOR3 V2)
{
	if (CMath::IsEqual(V1.x, V2.x) &&
		CMath::IsEqual(V1.y, V2.y) && 
		CMath::IsEqual(V1.z, V2.z))
	{
		return true;
	}
	return false;
}

bool CMath::IsEqual(TVECTOR4 V1, TVECTOR4 V2)
{
	if (CMath::IsEqual(V1.x, V2.x) &&
		CMath::IsEqual(V1.y, V2.y) &&
		CMath::IsEqual(V1.z, V2.z) &&
		CMath::IsEqual(V1.w, V2.w))
	{
		return true;
	}
	return false;
}

bool CMath::IsZero(float fA)
{
	return fabsf(fA) < EPSILON;
}

bool CMath::IsZero(TVECTOR2 V)
{
	if (IsZero(V.x) &&
		IsZero(V.y))
	{
		return true;
	}
	return false;
}

bool CMath::IsZero(TVECTOR3 V)
{
	if (IsZero(V.x) && 
		IsZero(V.y) &&
		IsZero(V.z))
	{
		return true;
	}
	return false;
}

bool CMath::IsZero(TVECTOR4 V)
{
	if (IsZero(V.x) &&
		IsZero(V.y) &&
		IsZero(V.z) &&
		IsZero(V.w))
	{
		return true;
	}
	return false;
}

CMath::TVECTOR3 CMath::Vector3Normalize(TVECTOR3 V)
{
	float dot = Vector3Magnitude(V);
	if (fabsf(dot) > EPSILON)
	{
		dot = 1 / dot;
		TVECTOR3 result = V;
		result.x *= dot;
		result.y *= dot;
		result.z *= dot;
		return result;
	}
	return V;
}

float CMath::Vector3Dot(TVECTOR3 V1, TVECTOR3 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z);
}

float CMath::Vector3Dot(TVECTOR4 V1, TVECTOR3 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z);
}

float CMath::Vector3Dot(TVECTOR3 V1, TVECTOR4 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z);
}

CMath::TVECTOR3 CMath::Vector3Cross(const TVECTOR3 V1, const TVECTOR3 V2)
{
	TVECTOR3 result;
	result.x = V1.y * V2.z - (V1.z * V2.y);
	result.y = V1.z * V2.x - (V1.x * V2.z);
	result.z = V1.x * V2.y - (V1.y * V2.x);
	return result;
}

float CMath::Vector3Magnitude(const TVECTOR3 V)
{
	return sqrtf(Vector3Dot(V, V));
}

CMath::TVECTOR3 CMath::Vector3Lerp(const TVECTOR3 V1, const TVECTOR3 V2, float fRatio)
{
	CMath::TVECTOR3 vResult = V2;
	vResult -= V1;
	vResult *= fRatio;
	vResult += V1;
	return vResult;
}

CMath::TVECTOR3 CMath::vector3Slerp(TVECTOR3 v0, TVECTOR3 v1, float t)
{
	// Compute the cosine of the angle between the two vectors.
	float dot = CMath::Vector3Dot(v0, v1);

	// If the dot product is negative, slerp won't take
	// the shorter path. Note that v1 and -v1 are equivalent when
	// the negation is applied to all four components. Fix by 
	// reversing one quaternion.
	if (dot < 0.0f)
	{
		v1 = -v1;
		dot = -dot;
	}

	const float DOT_THRESHOLD = 0.9995f;
	if (dot > DOT_THRESHOLD)
	{
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.

		CMath::TVECTOR3 result = (v1 - v0) + v0 * t;
		
		return result;
	}

	// Since dot is in range [0, DOT_THRESHOLD], acos is safe
	float theta_0 = acosf(dot);        // theta_0 = angle between input vectors
	float theta = theta_0 * t;          // theta = angle between v0 and result
	float sin_theta = sinf(theta);     // compute this value only once
	float sin_theta_0 = sinf(theta_0); // compute this value only once

	float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
	float s1 = sin_theta / sin_theta_0;

	return (v0 * s0) + (v1 * s1);
}

CMath::TVECTOR4::TVECTOR4()
{
	x = y = z = w = 0;
}

CMath::TVECTOR4::TVECTOR4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator+(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x + V.x;
	result.y = y + V.y;
	result.z = z + V.z;
	result.w = w + V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator-(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x - V.x;
	result.y = y - V.y;
	result.z = z - V.z;
	result.w = w - V.w;
	return result;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator-()
{
	// TODO: insert return statement here
	x = -x;
	y = -y;
	z = -z;
	w = -w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator+=(const TVECTOR4 V)
{
	x += V.x;
	y += V.y;
	z += V.z;
	w += V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator-=(const TVECTOR4 V)
{
	x -= V.x;
	y -= V.y;
	z -= V.z;
	w -= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator*=(const TVECTOR4 V)
{
	x *= V.x;
	y *= V.y;
	z *= V.z;
	w *= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator/=(const TVECTOR4 V)
{
	x /= V.x;
	y /= V.y;
	z /= V.z;
	w /= V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator*=(float S)
{
	x *= S;
	y *= S;
	z *= S;
	w *= S;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator/=(float S)
{
	x /= S;
	y /= S;
	z /= S;
	w /= S;
	return *this;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator*(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x * V.x;
	result.y = y * V.y;
	result.z = z * V.z;
	result.w = w * V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator/(const TVECTOR4 V)
{
	TVECTOR4 result;
	result.x = x / V.x;
	result.y = y / V.y;
	result.z = z / V.z;
	result.w = w / V.w;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator*(float S)
{
	TVECTOR4 result;
	result.x = x * S;
	result.y = y * S;
	result.z = z * S;
	result.w = w * S;
	return result;
}

CMath::TVECTOR4 CMath::TVECTOR4::operator/(float S)
{
	TVECTOR4 result;
	result.x = x / S;
	result.y = y / S;
	result.z = z / S;
	result.w = w / S;
	return result;
}

CMath::TVECTOR4& CMath::TVECTOR4::operator=(TVECTOR4 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	w = V.w;
	return *this;
}

CMath::TVECTOR4 & CMath::TVECTOR4::operator=(TVECTOR3 V)
{
	x = V.x;
	y = V.y;
	z = V.z;
	return *this;
}

bool CMath::TVECTOR4::operator==(TVECTOR4 V) const
{
	return IsEqual( *this, V );
}

bool CMath::TVECTOR4::operator!=(TVECTOR4 V) const
{
	return !( *this == V );
}

CMath::TVECTOR4 CMath::Vector4Normalize(TVECTOR4 V)
{
	float dot = Vector4Magnitude(V);
	if (fabsf(dot) > EPSILON)
	{
		dot = 1 / dot;
		TVECTOR4 result = V;
		result.x *= dot;
		result.y *= dot;
		result.z *= dot;
		result.x *= dot;
		return result;
	}
	return V;
}

float CMath::Vector4Dot(TVECTOR4 V1, TVECTOR4 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + V1.w * V2.w);
}

float CMath::Vector4Dot(TVECTOR4 V1, TVECTOR3 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + V1.w * 1);
}

float CMath::Vector4Dot(TVECTOR3 V1, TVECTOR4 V2)
{
	return (V1.x * V2.x + V1.y * V2.y + V1.z * V2.z + 1 * V2.w);
}

CMath::TVECTOR4 CMath::Vector4Cross(const TVECTOR4 V1, const TVECTOR4 V2)
{
	TVECTOR4 result;
	result.x = V1.y * V2.z - (V1.z * V2.y);
	result.y = V1.z * V2.x - (V1.x * V2.z);
	result.z = V1.x * V2.y - (V1.y * V2.x);
	result.w = 0;
	return result;
}

float CMath::Vector4Magnitude(const TVECTOR4 V)
{
	return sqrtf(Vector4Dot(V, V));
}

CMath::TMATRIX::TMATRIX()
{
	_11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 = _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = 0;
}

CMath::TMATRIX::TMATRIX(float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	_11 = m00;
	_12 = m01;
	_13 = m02;
	_14 = m03;
	_21 = m10;
	_22 = m11;
	_23 = m12;
	_24 = m13;
	_31 = m20;
	_32 = m21;
	_33 = m22;
	_34 = m23;
	_41 = m30;
	_42 = m31;
	_43 = m32;
	_44 = m33;
}

CMath::TMATRIX::TMATRIX(const TVECTOR4 V1, const TVECTOR4 V2, const TVECTOR4 V3, const TVECTOR4 V4)
{
	r[0] = V1;
	r[1] = V2;
	r[2] = V3;
	r[3] = V4;
}

CMath::TMATRIX CMath::TMATRIX::operator+(const TMATRIX M)
{
	CMath::TMATRIX result;
	result.r[0] = r[0] + M.r[0];
	result.r[1] = r[1] + M.r[1];
	result.r[2] = r[2] + M.r[2];
	result.r[3] = r[3] + M.r[3];
	return result;
}

CMath::TMATRIX CMath::TMATRIX::operator-(const TMATRIX M)
{
	CMath::TMATRIX result;
	result.r[0] = r[0] - M.r[0];
	result.r[1] = r[1] - M.r[1];
	result.r[2] = r[2] - M.r[2];
	result.r[3] = r[3] - M.r[3];
	return result;
}

CMath::TMATRIX & CMath::TMATRIX::operator+=(const TMATRIX M)
{
	r[0] += M.r[0];
	r[1] += M.r[1];
	r[2] += M.r[2];
	r[3] += M.r[3];
	return *this;
}

CMath::TMATRIX & CMath::TMATRIX::operator-=(const TMATRIX M)
{
	r[0] -= M.r[0];
	r[1] -= M.r[1];
	r[2] -= M.r[2];
	r[3] -= M.r[3];
	return *this;
}

CMath::TMATRIX & CMath::TMATRIX::operator*=(const TMATRIX M)
{
	CMath::TMATRIX tResult;
	for (int row = 0; row < 16; row += 4)
	{
		for (int col = 0; col < 4; col++)
		{
			tResult.mData[col + row] = mData[row] * M.mData[col] + mData[row + 1] * M.mData[col + 4] + mData[row + 2] * M.mData[col + 8] + mData[row + 3] * M.mData[col + 12];
		}
	}
	*this = tResult;
	return *this;
}

CMath::TMATRIX CMath::TMATRIX::operator*(const TMATRIX M)
{
	TMATRIX result;
	for (int row = 0; row < 16; row += 4)
	{
		for (int col = 0; col < 4; col++)
		{
			result.mData[col + row] = mData[row] * M.mData[col] + mData[row + 1] * M.mData[col + 4] + mData[row + 2] * M.mData[col + 8] + mData[row + 3] * M.mData[col + 12];
		}
	}
	return result;
}

CMath::TMATRIX CMath::TMATRIX::operator*(float S)
{
	TMATRIX result;
	result.r[0] = r[0] * S;
	result.r[1] = r[1] * S;
	result.r[2] = r[2] * S;
	result.r[3] = r[3] * S;
	return result;
}

CMath::TMATRIX & CMath::TMATRIX::operator=(const TMATRIX M)
{
	r[0] = M.r[0];
	r[1] = M.r[1];
	r[2] = M.r[2];
	r[3] = M.r[3];
	return *this;
}

bool CMath::TMATRIX::operator==(const TMATRIX M) const
{
	if (r[0] == M.r[0] &&
		r[1] == M.r[1] &&
		r[2] == M.r[2] &&
		r[3] == M.r[3])
	{
		return true;
	}
	return false;
}

float CMath::MatrixDeterminant(TMATRIX M)
{
	return
		M._11 * (M._22 * (M._33 * M._44 - M._43 * M._34) - M._23 * (M._32 * M._44 - M._42 * M._34) + M._24 * (M._32 * M._43 - M._42 * M._33)) -
		M._21 * (M._12 * (M._33 * M._44 - M._43 * M._34) - M._13 * (M._32 * M._44 - M._42 * M._34) + M._14 * (M._32 * M._43 - M._42 * M._33)) +
		M._31 * (M._12 * (M._23 * M._44 - M._43 * M._24) - M._13 * (M._22 * M._44 - M._42 * M._24) + M._14 * (M._22 * M._43 - M._42 * M._23)) -
		M._41 * (M._12 * (M._23 * M._34 - M._33 * M._24) - M._13 * (M._22 * M._34 - M._32 * M._24) + M._14 * (M._22 * M._33 - M._32 * M._23));

}

float CMath::MatrixDeterminant(float e_11, float e_12, float e_13, float e_21, float e_22, float e_23, float e_31, float e_32, float e_33)
{
	return e_11 * (e_22 * e_33 - e_32 * e_23) - e_12 * (e_21 * e_33 - e_31 * e_23) + e_13 * (e_21 * e_32 - e_31 * e_22);
}

CMath::TMATRIX CMath::MatrixTranspose(TMATRIX M)
{
	return { M._11,M._21,M._31,M._41,
			M._12,M._22,M._32,M._42,
			M._13,M._23,M._33,M._43,
			M._14,M._24,M._34,M._44 };
}

CMath::TMATRIX CMath::MatrixXRotation(float radian)
{
	float sinAngle = sinf(radian);
	float cosAngle = cosf(radian);

	return { 1,0,0,0,
			 0,cosAngle,sinAngle,0,
			 0,-sinAngle,cosAngle,0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixYRotation(float radian)
{
	float sinAngle = sinf(radian);
	float cosAngle = cosf(radian);

	return { cosAngle,0,-sinAngle,0,
			 0	,	1	,	0	,	0,
			 sinAngle,0,cosAngle,0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixZRotation(float radian)
{
	float sinAngle = sinf(radian);
	float cosAngle = cosf(radian);

	return { cosAngle,-sinAngle,0,0,
			 sinAngle,cosAngle,0,0,
			 0,0,1,0,
			 0,0,0,1 };
}

CMath::TMATRIX CMath::MatrixInverse(TMATRIX M)
{
	float det = MatrixDeterminant(M);
	if (fabs(det) < EPSILON)
		return M;
	TMATRIX a_transposed
	{
		 MatrixDeterminant(M._22, M._23, M._24, M._32, M._33, M._34, M._42, M._43, M._44),
		-MatrixDeterminant(M._12, M._13, M._14, M._32, M._33, M._34, M._42, M._43, M._44),
		 MatrixDeterminant(M._12, M._13, M._14, M._22, M._23, M._24, M._42, M._43, M._44),
		-MatrixDeterminant(M._12, M._13, M._14, M._22, M._23, M._24, M._32, M._33, M._34),

		-MatrixDeterminant(M._21, M._23, M._24, M._31, M._33, M._34, M._41, M._43, M._44),
		 MatrixDeterminant(M._11, M._13, M._14, M._31, M._33, M._34, M._41, M._43, M._44),
		-MatrixDeterminant(M._11, M._13, M._14, M._21, M._23, M._24, M._41, M._43, M._44),
		 MatrixDeterminant(M._11, M._13, M._14, M._21, M._23, M._24, M._31, M._33, M._34),

		 MatrixDeterminant(M._21, M._22, M._24, M._31, M._32, M._34, M._41, M._42, M._44),
		-MatrixDeterminant(M._11, M._12, M._14, M._31, M._32, M._34, M._41, M._42, M._44),
		 MatrixDeterminant(M._11, M._12, M._14, M._21, M._22, M._24, M._41, M._42, M._44),
		-MatrixDeterminant(M._11, M._12, M._14, M._21, M._22, M._24, M._31, M._32, M._34),

		-MatrixDeterminant(M._21, M._22, M._23, M._31, M._32, M._33, M._41, M._42, M._43),
		 MatrixDeterminant(M._11, M._12, M._13, M._31, M._32, M._33, M._41, M._42, M._43),
		-MatrixDeterminant(M._11, M._12, M._13, M._21, M._22, M._23, M._41, M._42, M._43),
		 MatrixDeterminant(M._11, M._12, M._13, M._21, M._22, M._23, M._31, M._32, M._33)
	};
	return a_transposed * (1 / det);
}

CMath::TMATRIX CMath::MatrixIdentity()
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixTranslation(float X, float Y, float Z)
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		X,Y,Z,1
	};
}

CMath::TMATRIX CMath::MatrixTranslationFromVector(TVECTOR4 V)
{
	return
		CMath::TMATRIX{
			1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		V.x,V.y,V.z,1
	};
}

CMath::TMATRIX CMath::MatrixTranslationFromVector(TVECTOR3 V)
{
	return
		CMath::TMATRIX{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		V.x,V.y,V.z,1
	};
}

CMath::TMATRIX CMath::MatrixScale(float fX, float fY, float fZ)
{
	return
		CMath::TMATRIX{
		fX,0,0,0,
		0,fY,0,0,
		0,0,fZ,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixScaleFromVector(TVECTOR3 V)
{
	return
		CMath::TMATRIX{
		V.x,0,0,0,
		0,V.y,0,0,
		0,0,V.z,0,
		0,0,0,1
	};
}

CMath::TMATRIX CMath::MatrixRotationQuaternion(TVECTOR4 V)
{
	CMath::TMATRIX result;

#define REALQUATERNION
#ifdef  REALQUATERNION
	//CHANGED
	CMath::TVECTOR4 v = V;

	float x = v.x, y = v.y, z = v.z, w = v.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	result.mData[0] = 1.0f - (yy + zz);
	result.mData[1] = xy - wz;
	result.mData[2] = xz + wy;
	result.mData[3] = 1.0f;

	result.mData[4] = xy + wz;
	result.mData[5] = 1.0f - (xx + zz);
	result.mData[6] = yz - wx;
	result.mData[7] = 1.0f;

	result.mData[8] = xz - wy;
	result.mData[9] = yz + wx;
	result.mData[10] = 1.0f - (xx + yy);
	result.mData[15] = 1.0f;

	result = CMath::MatrixTranspose(result);

#else
	float sqw = V.w*V.w;
	float sqx = V.x*V.x;
	float sqy = V.y*V.y;
	float sqz = V.z*V.z;

	float invs = 1.0f / (sqx + sqy + sqz + sqw);
	result._11 = (sqx - sqy - sqz + sqw)*invs;
	result._22 = (-sqx + sqy - sqz + sqw)*invs;
	result._33 = (-sqx - sqy + sqz + sqw)*invs;

	float tmp1 = V.x*V.y;
	float tmp2 = V.z*V.w;
	result._21 = 2.0f * (tmp1 + tmp2)*invs;
	result._12 = 2.0f * (tmp1 - tmp2)*invs;

	tmp1 = V.x*V.z;
	tmp2 = V.y*V.w;
	result._31 = 2.0f * (tmp1 - tmp2)*invs;
	result._13 = 2.0f * (tmp1 + tmp2)*invs;
	tmp1 = V.y*V.z;
	tmp2 = V.x*V.w;
	result._32 = 2.0f * (tmp1 + tmp2)*invs;
	result._23 = 2.0f * (tmp1 - tmp2)*invs;

	result._44 = 1.0f;
#endif

	return result;
}

CMath::TVECTOR4 CMath::QuaternionFromAxisAngle(TVECTOR3 axis, float degrees)
{
	float radians = degrees * 3.1416f / 180.0f;
	float fSinRad = sinf(radians * 0.5f);

	float x = axis.x * fSinRad;
	float y = axis.y * fSinRad;
	float z = axis.z * fSinRad;
	float w = cosf(radians * 0.5f);

	return TVECTOR4(x, y, z, w);
}

void CMath::toEulerAngle(const TVECTOR4 & q, float & roll, float & pitch, float & yaw)
{
	// roll (x-axis rotation)
	float sinr_cosp = +2.0f * ( q.w * q.x + q.y * q.z );
	float cosr_cosp = +1.0f - 2.0f * ( q.x * q.x + q.y * q.y );
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	float sinp = +2.0f * ( q.w * q.y - q.z * q.x );
	if(fabs(sinp) >= 1)
		pitch = copysignf(PI / 2, sinp); // use 90 degrees if out of range
	else
		pitch = asinf(sinp);

	// yaw (z-axis rotation)
	float siny_cosp = +2.0f * ( q.w * q.z + q.x * q.y );
	float cosy_cosp = +1.0f - 2.0f * ( q.y * q.y + q.z * q.z);
	yaw = atan2(siny_cosp, cosy_cosp);

}

CMath::TVECTOR4 CMath::QuaternionMultiply(TVECTOR4 a, TVECTOR4 b)
{
	TVECTOR4 result;

	result.x = b.w*a.x + b.x*a.w - b.y*a.z + b.z*a.y;
	result.y = b.w*a.y + b.x*a.z + b.y*a.w - b.z*a.x;
	result.z = b.w*a.z - b.x*a.y + b.y*a.x + b.z*a.w;
	result.w = b.w*a.w - b.x*a.x - b.y*a.y - b.z*a.z;

	return result;
}

CMath::TVECTOR3 CMath::Vector3Transform(TVECTOR3 tVector, TMATRIX tMatrix)
{
	TVECTOR3 tResult;

	tResult.x = tVector.x * tMatrix.r[0].mData[0] + tVector.y * tMatrix.r[1].mData[0] + tVector.z * tMatrix.r[2].mData[0];
	tResult.y = tVector.x * tMatrix.r[0].mData[1] + tVector.y * tMatrix.r[1].mData[1] + tVector.z * tMatrix.r[2].mData[1];
	tResult.z = tVector.x * tMatrix.r[0].mData[2] + tVector.y * tMatrix.r[1].mData[2] + tVector.z * tMatrix.r[2].mData[2];

	return tResult;
}

CMath::TVECTOR3 CMath::Point3Transform(TVECTOR3 tPoint, TMATRIX tMatrix)
{
	TVECTOR3 tResult;

	tResult.x = tPoint.x * tMatrix.r[0].mData[0] + tPoint.y * tMatrix.r[1].mData[0] + tPoint.z * tMatrix.r[2].mData[0] + tMatrix.r[3].mData[0];
	tResult.y = tPoint.x * tMatrix.r[0].mData[1] + tPoint.y * tMatrix.r[1].mData[1] + tPoint.z * tMatrix.r[2].mData[1] + tMatrix.r[3].mData[1];
	tResult.z = tPoint.x * tMatrix.r[0].mData[2] + tPoint.y * tMatrix.r[1].mData[2] + tPoint.z * tMatrix.r[2].mData[2] + tMatrix.r[3].mData[2];

	return tResult;
}

CMath::TVECTOR4 CMath::Vector4Transform(TVECTOR4 tVector, TMATRIX tMatrix)
{
	TVECTOR4 tResult;

	tResult.x = tVector.x * tMatrix.r[0].mData[0] + tVector.y * tMatrix.r[1].mData[0] + tVector.z * tMatrix.r[2].mData[0] + tVector.w * tMatrix.r[3].mData[0];
	tResult.y = tVector.x * tMatrix.r[0].mData[1] + tVector.y * tMatrix.r[1].mData[1] + tVector.z * tMatrix.r[2].mData[1] + tVector.w * tMatrix.r[3].mData[1];
	tResult.z = tVector.x * tMatrix.r[0].mData[2] + tVector.y * tMatrix.r[1].mData[2] + tVector.z * tMatrix.r[2].mData[2] + tVector.w * tMatrix.r[3].mData[2];
	tResult.w = tVector.x * tMatrix.r[0].mData[3] + tVector.y * tMatrix.r[1].mData[3] + tVector.z * tMatrix.r[2].mData[3] + tVector.w * tMatrix.r[3].mData[3];

	return tResult;
}

void CMath::MatrixDecompose(CMath::TVECTOR3 * vOutScale, CMath::TVECTOR4 * vOutRotationQuat, CMath::TVECTOR3 * vOutPosition, const CMath::TMATRIX M)
{
	CMath::TVECTOR3 vPos, vScale;
	vPos = *(CMath::TVECTOR3*)&M.r[3];
	if (vOutPosition != nullptr)
	{
		vOutPosition[0] = vPos;
	}

	vScale.x = CMath::Vector3Magnitude({ M.r[0].x, M.r[0].y, M.r[0].z });
	vScale.y = CMath::Vector3Magnitude({ M.r[1].x, M.r[1].y, M.r[1].z });
	vScale.z = CMath::Vector3Magnitude({ M.r[2].x, M.r[2].y, M.r[2].z });
	if (vOutScale != nullptr)
	{
		vOutScale[0] = vScale;
	}

	if (vOutRotationQuat != nullptr)
	{
		CMath::TMATRIX tTransp = CMath::MatrixScaleFromVector({ 1 / vScale.x, 1 / vScale.y, 1 / vScale.z }) * CMath::MatrixTranspose(M);
		float tr = tTransp.r[0].x + tTransp.r[1].y + tTransp.r[2].z;

		if (tr > 0)
		{
			float S = 0.5f / sqrtf(tr + 1.0f); // S=4*vOutRotationQuat.w 
			vOutRotationQuat[0].w = 0.25f / S;
			vOutRotationQuat[0].x = (tTransp.r[2].y - tTransp.r[1].z) * S;
			vOutRotationQuat[0].y = (tTransp.r[0].z - tTransp.r[2].x) * S;
			vOutRotationQuat[0].z = (tTransp.r[1].x - tTransp.r[0].y) * S;
		}
		else
		{
			if ((tTransp.r[0].x > tTransp.r[1].y) && (tTransp.r[0].x > tTransp.r[2].z))
			{
				float S = sqrtf(1.0f + tTransp.r[0].x - tTransp.r[1].y - tTransp.r[2].z) * 2; // S=4*vOutRotationQuat.x 
				vOutRotationQuat[0].w = (tTransp.r[2].y - tTransp.r[1].z) / S;
				vOutRotationQuat[0].x = 0.25f * S;
				vOutRotationQuat[0].y = (tTransp.r[0].y + tTransp.r[1].x) / S;
				vOutRotationQuat[0].z = (tTransp.r[0].z + tTransp.r[2].x) / S;
			}
			else if (tTransp.r[1].y > tTransp.r[2].z)
			{
				float S = sqrtf(1.0f + tTransp.r[1].y - tTransp.r[0].x - tTransp.r[2].z) * 2; // S=4*vOutRotationQuat.y
				vOutRotationQuat[0].w = (tTransp.r[0].z - tTransp.r[2].x) / S;
				vOutRotationQuat[0].x = (tTransp.r[0].y + tTransp.r[1].x) / S;
				vOutRotationQuat[0].y = 0.25f * S;
				vOutRotationQuat[0].z = (tTransp.r[1].z + tTransp.r[2].y) / S;
			}
			else
			{
				float S = sqrtf(1.0f + tTransp.r[2].z - tTransp.r[0].x - tTransp.r[1].y) * 2; // S=4*vOutRotationQuat.z
				vOutRotationQuat[0].w = (tTransp.r[1].x - tTransp.r[0].y) / S;
				vOutRotationQuat[0].x = (tTransp.r[0].z + tTransp.r[2].x) / S;
				vOutRotationQuat[0].y = (tTransp.r[1].z + tTransp.r[2].y) / S;
				vOutRotationQuat[0].z = 0.25f * S;
			}
		}
	}

}

float CMath::RandomFloat(float min, float max)
{
	return min +  ( (float)rand() / (float)RAND_MAX * (max - min)) ;
}

CMath::TVECTOR2::TVECTOR2()
{
	x = y = 0;
}

CMath::TVECTOR2::TVECTOR2(float fx, float fy)
{
	x = fx;
	y = fy;
}

std::ostream & operator<<(std::ostream & os, const CMath::TVECTOR2 V)
{
	return os << std::to_string(V.x)  << ", " << std::to_string(V.y);
}

std::ostream & operator<<(std::ostream & os, const CMath::TVECTOR3 V)
{
	return os << std::to_string(V.x) << ", " << std::to_string(V.y) << ", " << std::to_string(V.z);
}

std::ostream & operator<<(std::ostream & os, const CMath::TVECTOR4 V)
{
	return os << std::to_string(V.x) << ", " << std::to_string(V.y) << ", " << std::to_string(V.z) << ", " << std::to_string(V.w);
}

std::ostream & operator<<(std::ostream & os, const CMath::TMATRIX M)
{
	return os << "\n " <<
		M.r[0] << "\n " <<
		M.r[1] << "\n " <<
		M.r[2] << "\n " <<
		M.r[3] << '\n';
}
