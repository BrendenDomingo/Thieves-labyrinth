
Texture2D sourceTexture : register ( t0 );
SamplerState filter	: register( s0 );

struct INPUT
{
	float4 vPosition : SV_POSITION;
	float2 vUV : TEXCOORD;
};

float4 main(INPUT input) : SV_TARGET
{
	uint nTextureWidth = 0;
	uint nTextureHeight = 0;
	sourceTexture.GetDimensions(nTextureWidth, nTextureHeight);

	float fWeight[5] = { 1.0f, 0.9f, 0.55f, 0.18f, 0.1f };

	float fNormalize = 1.0f / ( fWeight[0] + 2.0f * ( fWeight[1] + fWeight[2] + fWeight[3] + fWeight[4] ) );

	fWeight[0] *= fNormalize;
	fWeight[1] *= fNormalize;
	fWeight[2] *= fNormalize;
	fWeight[3] *= fNormalize;
	fWeight[4] *= fNormalize;

	float4 vColor = float4( 0.0f, 0.0f, 0.0f, 0.0f );

	float fTexelSize = 1.0f / nTextureWidth;

	float2 vNeighborUV[9];

	int nOffset = -4;
	for(int i = 0; i < 9; i++)
	{
		vNeighborUV[i] = input.vUV + float2( fTexelSize * nOffset, 0.0f );
		nOffset++;
	}

	vColor += sourceTexture.Sample(filter, vNeighborUV[0] ) * fWeight[4];
	vColor += sourceTexture.Sample(filter, vNeighborUV[1] ) * fWeight[3];
	vColor += sourceTexture.Sample(filter, vNeighborUV[2] ) * fWeight[2];
	vColor += sourceTexture.Sample(filter, vNeighborUV[3] ) * fWeight[1];
	vColor += sourceTexture.Sample(filter, vNeighborUV[4] ) * fWeight[0];
	vColor += sourceTexture.Sample(filter, vNeighborUV[5] ) * fWeight[1];
	vColor += sourceTexture.Sample(filter, vNeighborUV[6] ) * fWeight[2];
	vColor += sourceTexture.Sample(filter, vNeighborUV[7] ) * fWeight[3];
	vColor += sourceTexture.Sample(filter, vNeighborUV[8] ) * fWeight[4];

	vColor.a = 1.0f;

	return vColor;
}