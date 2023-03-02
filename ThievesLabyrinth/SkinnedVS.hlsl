#pragma pack_matrix(row_major)
#define MAX_JOINTS 35

Texture2D AnimPositions : register( t0 );
Texture2D AnimRotations : register( t1 );
SamplerState Sampler : register( s0 );


cbuffer Model : register ( b0 )
{
	float4x4 modeling;
};

cbuffer Camera : register ( b1 )
{
	float4x4 view;
	float4x4 projection;
}

cbuffer AnimationBuffer : register ( b2 )
{
	float4x4 mBindpose[MAX_JOINTS];
	float	 fCurrentTime;
	float	 fTotalTime;

	int		 nJointCount;
	int		 nCurrentAnimation;
	int		 nFrameCount;
}

struct VERTEX
{
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vUV : TEXCOORD;
	float4 vJointIndex : JOINT_INDEX;
	float4 vWeights : WEIGHTS;
	float4 vTangent : TANGENT;
};

struct OUTPUT
{
	float4 vPosition : SV_POSITION;
	float2 vUV : TEXCOORD;
	float3 vWorldPos : WPOSITION;
	float3 vNormal : NORMAL;
	float2 vNDC : NDCPOS;
	float3 vTangent : TANGENT;
};


float4x4 QuaternionToMatrix(float4 quat)
{
	float4x4 m = float4x4( float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ), float4( 0, 0, 0, 0 ) );

	float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	m[0][0] = 1.0 - ( yy + zz );
	m[0][1] = xy - wz;
	m[0][2] = xz + wy;

	m[1][0] = xy + wz;
	m[1][1] = 1.0 - ( xx + zz );
	m[1][2] = yz - wx;

	m[2][0] = xz - wy;
	m[2][1] = yz + wx;
	m[2][2] = 1.0 - ( xx + yy );

	m[3][3] = 1.0;

	return transpose(m);
}

float4x4 GetMatrixFromTexture(float2 vUV)
{
	float4 jointRotation = AnimRotations.SampleLevel(Sampler, vUV, 0);
	float4 jointPosition = AnimPositions.SampleLevel(Sampler, vUV, 0);

	float4x4 jointMatrix = QuaternionToMatrix(jointRotation);

	jointMatrix[3] = jointPosition;

	return jointMatrix;
}


OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	float4 vSkinnedPos = float4( 0, 0, 0, 0 );
	float3 vSkinnedNorm = float3( 0, 0, 0 );
	float3 vSkinnedTangent = float3( 0, 0, 0 );

	input.vPosition.w = 1.0f;
	input.vTangent.w = 1.0f;

	for(int i = 0; i < 4; i++)
	{
		int nTextureWidth;
		int nTextureHeight;
		AnimPositions.GetDimensions(nTextureWidth, nTextureHeight);

		float fLerpAdjust = (nFrameCount-1) / ( nTextureWidth + 0.0f );
		float fRatio = fCurrentTime / fTotalTime;

		float2 vUV = float2( fRatio * fLerpAdjust, ( input.vJointIndex[i] + ( nJointCount * nCurrentAnimation ) ) / ( nTextureHeight - 0.5f ) );

		float4x4 joint = mul(mBindpose[input.vJointIndex[i]], GetMatrixFromTexture(vUV));

		vSkinnedPos += mul(input.vPosition, joint) * input.vWeights[i];
		vSkinnedNorm += mul(float4(input.vNormal, 0.0f), joint * input.vWeights[i]).xyz;
		vSkinnedTangent += mul(input.vTangent, joint * input.vWeights[i]).xyz;
	}

	output.vPosition = mul(vSkinnedPos, modeling);
	output.vWorldPos = output.vPosition.xyz;

	output.vPosition = mul(output.vPosition, view);
	output.vNDC.x = output.vPosition.x / output.vPosition.w;
	output.vNDC.y = output.vPosition.y / output.vPosition.w;
	output.vPosition = mul(output.vPosition, projection);



	output.vNormal = normalize(mul(float4(vSkinnedNorm, 0.0f), modeling).xyz);
	output.vTangent = normalize(mul(float4(vSkinnedTangent, 0.0f), modeling).xyz);
	output.vUV = input.vUV;

	return output;
}