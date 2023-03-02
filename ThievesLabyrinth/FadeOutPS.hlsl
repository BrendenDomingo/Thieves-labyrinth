Texture2D textureResource : register ( t0 );
SamplerState filter : register ( s0 );

cbuffer DataBuffer : register( b0 )
{
	float fTime;
	float3 vPadding;
}


float4 main(float4 vPosition : SV_POSITION, float2 vUV : TEXCOORD) : SV_TARGET
{
	float4 vColor = textureResource.Sample(filter, vUV);

	if(vColor.a > 0.0f)
	{
		vColor.r += fTime;
		vColor.a = fTime;
	}

	return  vColor;
}