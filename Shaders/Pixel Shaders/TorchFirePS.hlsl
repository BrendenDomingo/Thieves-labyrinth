
Texture2D noise : register( t0 );
Texture2D fade  : register( t1 );
Texture2D fireShape	: register( t2 );
SamplerState filter	: register( s0 );

cbuffer UtilityBuffer : register( b0 )
{
	float fTime;
	float fMaskValue;
	float2 vPadding;
}

struct INPUT
{
	float4 vPosition : SV_POSITION;
	float2 vUV : TEXTURECOORD;
	float3 vWorldPos : WPOSITION;
	float3 vNormal : NORMAL;
	float2 vNDC : NDCPOS;
	float3 vTangent : TANGENT;
};

float4 main(INPUT input) : SV_TARGET
{
	float4 vFinalColor = float4( 0,0,0,0 );
	float xOffset = clamp(sin(fTime) * .2, -input.vUV.y, input.vUV.y);

	float2 vScrollingUV = float2(input.vUV.x + input.vUV.y * xOffset, input.vUV.y - fTime * .5f);
	float4 vPerlinNoise = noise.Sample( filter, vScrollingUV );

	float4 vFlameShapeColor = fireShape.Sample(filter, vScrollingUV );

	vFinalColor.g += fade.Sample(filter, -input.vUV ).r * 1.5 * vPerlinNoise.r;


	if(vPerlinNoise.r * noise.Sample(filter, input.vUV ).r < .01 + fMaskValue * fade.Sample( filter, input.vUV ).r)
		discard;

	vScrollingUV = float2(vScrollingUV.x, 1 - input.vUV.y);

	vScrollingUV.x += fade.Sample( filter, input.vUV ).r * ( sin(fTime * 4) * ( .4 * vFlameShapeColor.y ) ) * vPerlinNoise.r;

	if(vFlameShapeColor.r < .2)
		discard;

	return vFinalColor;
}