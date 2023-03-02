#pragma pack_matrix(row_major)

cbuffer Model : register ( b0 )
{
	float4x4 modeling;
};

cbuffer Camera: register( b1 )
{
	float4x4 view;
	float4x4 projection;
}

struct VERTEX
{
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vUV : TEXCOORD;
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


OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;
	input.vPosition.w = 1;
	input.vTangent.w = 1.0f;

	output.vPosition = mul(input.vPosition, modeling);
	output.vWorldPos = output.vPosition.xyz;

	output.vPosition = mul(output.vPosition, view);
	output.vNDC.x = output.vPosition.x / output.vPosition.w;
	output.vNDC.y = output.vPosition.y / output.vPosition.w;

	output.vPosition = mul(output.vPosition, projection);

	output.vNormal = normalize(mul(float4(input.vNormal, 0.0f), modeling).xyz);
	output.vTangent = normalize(mul(input.vTangent, modeling).xyz);

	output.vUV = input.vUV;

	return output;
}