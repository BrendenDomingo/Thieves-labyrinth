Texture2D textureResource : register ( t0 );
SamplerState filter : register ( s0 );

cbuffer DataBuffer : register( b0 )
{
	float fTime;
	float fLifetime;
	float3 vPadding;
}

struct INPUT
{
	float4 vPosition : SV_POSITION;
	float2 vUV : TEXCOORD;
	float3 vWorldPos : WPOSITION;
	float3 vNormal : NORMAL;
	float2 vNDC : NDCPOS;
	float3 vTangent : TANGENT;
};

float4 main(INPUT input) : SV_TARGET
{
	float2 vUV = float2(input.vUV.x + fTime, (input.vUV.y - .8f) - fLifetime);

	float4 vTextureColor = textureResource.Sample(filter, vUV);

	return vTextureColor * vTextureColor * 2;
}