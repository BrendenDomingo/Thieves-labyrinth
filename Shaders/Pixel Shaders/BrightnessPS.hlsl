Texture2D textureResource : register ( t0 );
SamplerState filter : register ( s0 );

cbuffer BrightnessBuffer : register( b0 )
{
	float fBrightness;
	float vPadding[3];
}


float4 main(float4 vPosition : SV_POSITION, float2 vUV : TEXCOORD) : SV_TARGET
{
	float4 vColor = textureResource.Sample(filter, vUV) * fBrightness;

	return  vColor;
}