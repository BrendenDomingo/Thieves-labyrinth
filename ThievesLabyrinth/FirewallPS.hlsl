Texture2D LavaNoise : register ( t0 );
Texture2D noise : register ( t1 );
SamplerState filter : register ( s0 );

cbuffer UtilityBuffer : register( b0 )
{
	float fTime;
	float3 vPadding;
}

float4 main (float4 vPosition : SV_POSITION, float2 vUV : TEXCOORD) : SV_TARGET
{
	float2 vNewUV = float2(vUV.x + sin(fTime * .2) * .5, vUV.y + fTime * .2);

	float2 vScrollingUV = float2(vNewUV.x + sin(fTime * .1) * .1f, LavaNoise.Sample( filter, vNewUV ).r);

	float vNoiseA = LavaNoise.Sample( filter, vScrollingUV ).r;

	/*if(noise.Sample(filter, vNewUV).r * vNewUV.y < .1f)
		discard;
*/
	float4 vFinalColor = float4(1, 0, 0, vNoiseA);

	vFinalColor.g += 1 * vUV.y * vNoiseA;

	return vFinalColor;
}