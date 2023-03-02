Texture2D textureResource : register ( t0 );
SamplerState filter : register ( s0 );

cbuffer UtilityBuffer : register( b0 )
{
	float fCurrentLife;
	float fMaxLife;
	float2 vPadding;
}

float4 main(float4 vPos : SV_POSITION, float2 vUV : TEXCOORD) : SV_TARGET
{
	float4 vFinalColor = textureResource.Sample(filter,vUV);

	float fAlpha = fCurrentLife / fMaxLife;

	if(vFinalColor.a > .2f)
	{
		vFinalColor.a = fAlpha;
	}

	return vFinalColor;
}