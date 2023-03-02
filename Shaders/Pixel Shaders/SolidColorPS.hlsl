cbuffer ColorBuffer : register ( b0 )
{
	float4 vBaseColor;
}

struct INPUT
{
	float4 pos : SV_POSITION;
};

float4 main(INPUT input) : SV_TARGET
{
	return vBaseColor;
}