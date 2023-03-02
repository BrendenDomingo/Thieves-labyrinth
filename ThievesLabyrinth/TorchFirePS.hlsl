
Texture2D fade : register( t0 ); //fireFlame
Texture2D noise  : register( t1 ); //Noise
Texture2D flameShape : register( t2 ); // fireShape
SamplerState filter	: register( s0 );

cbuffer UtilityBuffer : register( b0 )
{
	float fTime;
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
	/*float2 vFlameUV = float2(input.vUV.x + sin(fTime) * .1, input.vUV.y - ( fTime *.1 ));

	float4 vFinalColor = fireColor.Sample(filter, vFlameUV);

	float4 vAshMask = noise.Sample(filter, vFlameUV * .5 );

	vFlameUV.x = input.vUV.x - sin(fTime) * .1f;
	vFinalColor *= fireColor.Sample(filter, vFlameUV );

	vFlameUV.y = sin(input.vUV.x * 2) * input.vUV.y * .2f;
	vAshMask *= noise.Sample(filter , vFlameUV * .2f );

	vAshMask.a = vAshMask.r;

	vFinalColor *= vAshMask.a * 2;

	float xOffset = sin(fTime) * .05;

	float4 flameMask = fireShape.Sample(filter, float2(input.vUV.x + xOffset,  input.vUV.y) );

	vFinalColor.a = flameMask.r;

	if(vFinalColor.x == vFinalColor.y  && vFinalColor.x == vFinalColor.z)
		vFinalColor.a = 0;

	vFinalColor.g *= ( 1 - input.vUV.y ) * 10;

	vFinalColor = ( ( ( vFinalColor * noise.Sample( filter, input.vUV ) * .5 ) * 2 * flameMask ) * 2 ) * 2 * vFinalColor;*/

	/*float2 vUV = float2(input.vUV.x  + sin(fTime) * (1 - input.vUV.y) * .05, input.vUV.y - sin(fTime) * input.vUV.y * .02);

	float4 vFinalColor = fireColor.Sample( filter, vUV );*/

	/*float4 vFinalColor = float4(1,0,0,1);
	float xOffset = clamp(sin(fTime) * .2, -input.vUV.y, input.vUV.y);

	float2 uv = float2(input.vUV.x + input.vUV.y * xOffset, input.vUV.y + fTime * .5f );
	float4 perlinNoise = noise.Sample( filter, uv );

	perlinNoise *= perlinNoise;

	float4 flameShapeColor = flameShape.Sample( filter, uv );

	vFinalColor.g += fade.Sample( filter, -input.vUV ).r * 1.5 * perlinNoise.r;

	if(perlinNoise.r < .01 + .7 * fade.Sample(filter, input.vUV).r)
	{
		vFinalColor.a = perlinNoise.r;
	}

	uv = float2(uv.x, input.vUV.y);

	uv.x += fade.Sample(filter, input.vUV ).r * ( sin(fTime * 4) * ( .4 * flameShapeColor.y ) ) * perlinNoise.r;

	flameShapeColor = flameShape.Sample( filter, uv );
	
	vFinalColor.a *= flameShapeColor.r * fade.Sample(filter, input.vUV).r;

	return vFinalColor;*/


	float4 vFinalColor = float4( 1,0,0,1 );
	float2 vScrollingUV = float2(input.vUV.x , input.vUV.y + fTime );


	float fFade = fade.Sample(filter, input.vUV).r;
	float fNoise = noise.Sample(filter, vScrollingUV).r;
	
	vScrollingUV.x += fNoise;

	//Flames
	vFinalColor.a = fNoise < 0.7f * fFade ? 1 : 0;

	if(fNoise > 0.7f * fFade)
		discard;

	//Color
	vFinalColor.g +=  fFade * fFade * 1.5f * fNoise;

	//Flame Shape
	
	float2 vMaskUV = float2( input.vUV.x + fFade * sin(fTime) * fNoise * .5f, input.vUV.y);
	float fShape = flameShape.Sample(filter, vMaskUV).r + flameShape.Sample(filter, input.vUV).r;

	vFinalColor.a *= fShape;

	vFinalColor.a = 1;

	return vFinalColor;
}