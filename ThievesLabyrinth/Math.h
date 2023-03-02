#pragma once
////////////////////////////////////////////////////////////
// Filename:			Math.h
// Date:				02/12/2019
// Mod Date:			--
// Mod Initials:		--
// Author:				Tylor Ford
// Purpose:				Defines Math Types 
//						and functions
////////////////////////////////////////////////////////////
#include <iostream>

#define EPSILON 0.00001f
#define PI 3.14159265359f
#define TOANGLE(rad) (rad / PI * 180.0f)
#define TORAD(deg) (deg * PI / 180.0f)

class CMath
{
public:
	struct alignas(8) TVECTOR2;
	struct alignas(16) TVECTOR3;
	struct alignas(16) TVECTOR4;
	struct alignas(64) TMATRIX;


	// Vector of 2 floats	x, y 
	struct alignas(8) TVECTOR2
	{
		union 
		{
			float mData[2];
			struct
			{
				float x, y;
			};
		};
		TVECTOR2();
		TVECTOR2(float fx, float fy);

	};

	// Vector of 3 floats	x, y, z
	struct alignas(16) TVECTOR3
	{
		union
		{
			float mData[3];
			struct
			{
				float x, y, z;
			};
		};
		TVECTOR3();
		TVECTOR3(float _x, float _y, float _z);

		CMath::TVECTOR3 operator+(const TVECTOR3 V);
		CMath::TVECTOR3 operator-(const TVECTOR3 V);

		CMath::TVECTOR3 operator-();

		CMath::TVECTOR3& operator+=(const TVECTOR3 V);
		CMath::TVECTOR3& operator-=(const TVECTOR3 V);
		CMath::TVECTOR3& operator*=(const TVECTOR3 V);
		CMath::TVECTOR3& operator/=(const TVECTOR3 V);

		CMath::TVECTOR3& operator*=(float S);
		CMath::TVECTOR3& operator/=(float S);

		CMath::TVECTOR3 operator*(const TVECTOR3 V);
		CMath::TVECTOR3 operator/(const TVECTOR3 V);
		CMath::TVECTOR3 operator*(float S);
		CMath::TVECTOR3 operator/(float S);

		CMath::TVECTOR3& operator=(TVECTOR3 v);
		CMath::TVECTOR3& operator=(TVECTOR4 V);

		bool operator==(TVECTOR3 V) const;
		bool operator!=(TVECTOR3 V) const;
	};

	// Vector of 4 floats	x, y, z, w
	struct alignas(16) TVECTOR4
	{
		union
		{
			float mData[4];
			struct
			{
				float x, y, z, w;
			};
		};
		TVECTOR4();
		TVECTOR4(float _x, float _y, float _z, float _w);

		CMath::TVECTOR4 operator+(const TVECTOR4 V);
		CMath::TVECTOR4 operator-(const TVECTOR4 V);

		CMath::TVECTOR4& operator-();

		CMath::TVECTOR4& operator+=(const TVECTOR4 V);
		CMath::TVECTOR4& operator-=(const TVECTOR4 V);
		CMath::TVECTOR4& operator*=(const TVECTOR4 V);
		CMath::TVECTOR4& operator/=(const TVECTOR4 V);

		CMath::TVECTOR4& operator*=(float S);
		CMath::TVECTOR4& operator/=(float S);

		CMath::TVECTOR4 operator*(const TVECTOR4 V);
		CMath::TVECTOR4 operator/(const TVECTOR4 V);
		CMath::TVECTOR4 operator*(float S);
		CMath::TVECTOR4 operator/(float S);

		CMath::TVECTOR4& operator=(TVECTOR4 V);
		CMath::TVECTOR4& operator=(TVECTOR3 V);

		bool operator==(TVECTOR4 V) const;
		bool operator!=(TVECTOR4 V) const;

	};

	// Matrix with 4 Vector4's (x-axis, y-axis, z-axis, translation) | 16 floats
	struct alignas(64) TMATRIX
	{
		union
		{
			TVECTOR4 r[4];
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float mData[16];
		};
		TMATRIX();
		TMATRIX(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33);
		TMATRIX(const TVECTOR4 V1, const TVECTOR4 V2, const TVECTOR4 V3, const TVECTOR4 V4);
		

		CMath::TMATRIX operator+(const TMATRIX M);
		CMath::TMATRIX operator-(const TMATRIX M);

		CMath::TMATRIX& operator+=(const TMATRIX M);
		CMath::TMATRIX& operator-=(const TMATRIX M);
		CMath::TMATRIX& operator*=(const TMATRIX M);

		CMath::TMATRIX operator*(const TMATRIX M);
		CMath::TMATRIX operator*(float S);

		CMath::TMATRIX& operator=(const TMATRIX M);
		bool operator==(const TMATRIX M) const;
	};

	/************************************* Math Functions *************************************************/

	///////////////////////////////////////////////////////////////////
	// IsEqual()		 
	// in:	float, float
	// out: bool
	// returns whether two floats are equal
	///////////////////////////////////////////////////////////////////
	static bool IsEqual(float fA, float fB);

	///////////////////////////////////////////////////////////////////
	// IsEqual()		 
	// in:	tVector2, tVector2
	// out: bool
	// returns whether two tVector2's are equal
	///////////////////////////////////////////////////////////////////
	static bool IsEqual(TVECTOR2 V1, TVECTOR2 V2);

	///////////////////////////////////////////////////////////////////
	// IsEqual()		 
	// in:	tVector3, tVector3
	// out: bool
	// returns whether two tVector3's are equal
	///////////////////////////////////////////////////////////////////
	static bool IsEqual(TVECTOR3 V1, TVECTOR3 V2);

	///////////////////////////////////////////////////////////////////
	// IsEqual()		 
	// in:	tVector4, tVector4
	// out: bool
	// returns whether two tVector4's are equal
	///////////////////////////////////////////////////////////////////
	static bool IsEqual(TVECTOR4 V1, TVECTOR4 V2);

	///////////////////////////////////////////////////////////////////
	// IsZero()		 
	// in:	float
	// out: bool
	// returns whether a float is equal to zero
	///////////////////////////////////////////////////////////////////
	static bool IsZero(float fA);	

	///////////////////////////////////////////////////////////////////
	// IsZero()		 
	// in:	TVECTOR2
	// out: bool
	// returns whether a Vector2 is equal to zero
	///////////////////////////////////////////////////////////////////
	static bool IsZero(TVECTOR2 V);

	///////////////////////////////////////////////////////////////////
	// IsZero()		 
	// in:	TVECTOR3
	// out: bool
	// returns whether a Vector3 is equal to zero
	///////////////////////////////////////////////////////////////////
	static bool IsZero(TVECTOR3 V);

	///////////////////////////////////////////////////////////////////
	// IsZero()		 
	// in:	TVECTOR4
	// out: bool
	// returns whether a Vector4 is equal to zero
	///////////////////////////////////////////////////////////////////
	static bool IsZero(TVECTOR4 V);

	/******************************************************************************************************/


	/************************************* Vector3 ********************************************************/

	///////////////////////////////////////////////////////////////////
	// Vector3Normailize()		 
	// in:	TVECTOR3
	// out: TVECTOR3
	// returns normalized Vector3 from passed in Vector3
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR3 Vector3Normalize(TVECTOR3 V);

	///////////////////////////////////////////////////////////////////
	// Vector3Dot()		 
	// in:	TVECTOR3, TVECTOR3
	// out: FLOAT
	// returns dot product from two Vector3's
	///////////////////////////////////////////////////////////////////
	static float Vector3Dot(TVECTOR3 V1, TVECTOR3 V2);

	///////////////////////////////////////////////////////////////////
	// Vector3Dot()		 
	// in:	TVECTOR4, TVECTOR3
	// out: FLOAT
	// returns dot product from a Vector3 and a vector4 used as a v3
	///////////////////////////////////////////////////////////////////
	static float Vector3Dot(TVECTOR4 V1, TVECTOR3 V2);

	///////////////////////////////////////////////////////////////////
	// Vector3Dot()		 
	// in:	TVECTOR3, TVECTOR4
	// out: FLOAT
	// returns dot product from a Vector3 and a vector4 used as a v3
	///////////////////////////////////////////////////////////////////
	static float Vector3Dot(TVECTOR3 V1, TVECTOR4 V2);

	///////////////////////////////////////////////////////////////////
	// Vector3Cross()	 
	// in:	TVECTOR3, TVECTOR3
	// out: TVECTOR3
	// retuns crossed Vector3 from two passed in Vector3's
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR3 Vector3Cross(const TVECTOR3 V1, const TVECTOR3 V2);

	///////////////////////////////////////////////////////////////////
	// Vector3Magnitude()	 
	// in:	TVECTOR3
	// out: float
	// retuns a Vector3's magnitude 
	///////////////////////////////////////////////////////////////////
	static float Vector3Magnitude(const TVECTOR3 V);

	///////////////////////////////////////////////////////////////////
	// Vector3Lerp()	 
	// in:	TVECTOR3, TVECTOR3, float
	// out: TVECTOR3
	// retuns a Vector3 lerped between two vectors
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR3 Vector3Lerp(const TVECTOR3 V1, const TVECTOR3 V2, float fRatio);

	static CMath::TVECTOR3 vector3Slerp(TVECTOR3 v0, TVECTOR3 v1, float t);

	/******************************************************************************************************/


	/************************************* Vector4 ********************************************************/

	///////////////////////////////////////////////////////////////////
	// Vector4Normailize()		 
	// in:	TVECTOR4
	// out: TVECTOR4
	// returns normalizes Vector4 from a passed in Vector4
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 Vector4Normalize(TVECTOR4 V);

	///////////////////////////////////////////////////////////////////
	// Vector4Dot()		 
	// in:	TVECTOR4, TVECTOR4
	// out: FLOAT
	// returns dot product from passed in Vector4's
	///////////////////////////////////////////////////////////////////
	static float Vector4Dot(TVECTOR4 V1, TVECTOR4 V2);

	///////////////////////////////////////////////////////////////////
	// Vector4Dot()		 
	// in:	TVECTOR4, TVECTOR3
	// out: FLOAT
	// returns dot product from a Vector4 and a vector3 with w = 1
	///////////////////////////////////////////////////////////////////
	static float Vector4Dot(TVECTOR4 V1, TVECTOR3 V2);

	///////////////////////////////////////////////////////////////////
	// Vector4Dot()		 
	// in:	TVECTOR3, TVECTOR4
	// out: FLOAT
	// returns dot product from a Vector4 and a vector3 with w = 1
	///////////////////////////////////////////////////////////////////
	static float Vector4Dot(TVECTOR3 V1, TVECTOR4 V2);

	///////////////////////////////////////////////////////////////////
	// Vector4Cross()		 
	// in:	TVECTOR4, TVECTOR4
	// out: TVECTOR4
	// returns crossed Vector4 from two passed in Vector4's
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 Vector4Cross(const TVECTOR4 V1, const TVECTOR4 V2);

	///////////////////////////////////////////////////////////////////
	// Vector4Magnitude()	 
	// in:	TVECTOR3
	// out: float
	// retuns a Vector3's magnitude 
	///////////////////////////////////////////////////////////////////
	static float Vector4Magnitude(const TVECTOR4 V);

	/******************************************************************************************************/


	/************************************* Matrix *********************************************************/

	///////////////////////////////////////////////////////////////////
	// MatrixDeterminant()		 
	// in:	TMATRIX
	// out: FLOAT
	// returns Matrix determinant
	///////////////////////////////////////////////////////////////////
	static float MatrixDeterminant(TMATRIX M);

	///////////////////////////////////////////////////////////////////
	// MatrixDeterminant()		 
	// in:	9 FLOATS 
	// out: FLOAT
	// returns determinant of passed in floats
	///////////////////////////////////////////////////////////////////
	static float MatrixDeterminant(float e_11, float e_12, float e_13,
		float e_21, float e_22, float e_23,
		float e_31, float e_32, float e_33);

	///////////////////////////////////////////////////////////////////
	// MatrixTranspose()	 
	// in:	TMATRIX
	// out: TMATRIX
	// returns transposed Matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranspose(TMATRIX M);

	///////////////////////////////////////////////////////////////////
	// MatrixXRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on x-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixXRotation(float radian);

	///////////////////////////////////////////////////////////////////
	// MatrixYRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on y-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixYRotation(float radian);

	///////////////////////////////////////////////////////////////////
	// MatrixZRotation()	 
	// in:	float
	// out: TMATRIX
	// returns rotated Matrix on z-axis
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixZRotation(float radian);

	///////////////////////////////////////////////////////////////////
	// MatrixInverse()	 
	// in:	TMATRIX
	// out: TMATRIX
	// returns inversed matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixInverse(TMATRIX M);

	///////////////////////////////////////////////////////////////////
	// MatrixIdentity()	 
	// in:	void
	// out: TMATRIX
	// returns an identity Matrix
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixIdentity();

	///////////////////////////////////////////////////////////////////
	// MatrixTranslation()	 
	// in:	float, float, float
	// out: TMATRIX
	// returns a translated Matrix by passed in values
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslation(float X, float Y, float Z);

	///////////////////////////////////////////////////////////////////
	// MatrixTranslationFromVector()	 
	// in:	TVECTOR4
	// out: TMATRIX
	// returns a translated Matrix from a TVECTOR4
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslationFromVector(TVECTOR4 V);

	///////////////////////////////////////////////////////////////////
	// MatrixTranslationFromVector()	 
	// in:	TVECTOR3
	// out: TMATRIX
	// returns a translated Matrix from a TVECTOR3
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixTranslationFromVector(TVECTOR3 V);

	static CMath::TMATRIX MatrixScale(float fX, float fY, float fZ);
	static CMath::TMATRIX MatrixScaleFromVector(TVECTOR3 V);

	///////////////////////////////////////////////////////////////////
	// MatrixRotationQuaternion()	 
	// in:	TVECTOR4
	// out: TMATRIX
	// returns a rotated Matrix from a TVECTOR4
	///////////////////////////////////////////////////////////////////
	static CMath::TMATRIX MatrixRotationQuaternion(TVECTOR4 V);

	///////////////////////////////////////////////////////////////////
	// QuaternionFromAxisAngle()	 
	// in:	TVECTOR3, float
	// out: TVECTOR4
	// returns a quaternion for a rotation of the given degrees around the given axis
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 QuaternionFromAxisAngle(TVECTOR3 axis, float degrees);

	static void toEulerAngle(const TVECTOR4& q, float& roll, float& pitch, float& yaw);

	///////////////////////////////////////////////////////////////////
	// QuaternionMultiply()	 
	// in:	TVECTOR4, TVECTOR4
	// out:	TVECTOR4
	// returns the product of two quaternions
	///////////////////////////////////////////////////////////////////
	static CMath::TVECTOR4 QuaternionMultiply(TVECTOR4 a, TVECTOR4 b);

	static CMath::TVECTOR3 Vector3Transform(TVECTOR3 tVector, TMATRIX tMatrix);

	static CMath::TVECTOR3 Point3Transform(TVECTOR3 tPoint, TMATRIX tMatrix);

	static CMath::TVECTOR4 Vector4Transform(TVECTOR4 tVector, TMATRIX tMatrix);

	static void MatrixDecompose(CMath::TVECTOR3* vOutScale, CMath::TVECTOR4* vOutRotationQuat, CMath::TVECTOR3* vOutPosition, const CMath::TMATRIX M);


	/******************************************************************************************************/


	/************************************* Other Math *********************************************************/

	static float RandomFloat(float min, float max);
};
//	Returns "X, Y"
std::ostream& operator<<(std::ostream& os, const CMath::TVECTOR2 V);
//	Returns "X, Y, Z"
std::ostream& operator<<(std::ostream& os, const CMath::TVECTOR3 V);
//	Returns "X, Y, Z, W"
std::ostream& operator<<(std::ostream& os, const CMath::TVECTOR4 V);
//	Returns "X1, Y1, Z1, W1"
//			"X2, Y2, Z2, W2"
//			"X3, Y3, Z3, W3"
//			"X4, Y4, Z4, W4"
std::ostream& operator<<(std::ostream& os, const CMath::TMATRIX M);
