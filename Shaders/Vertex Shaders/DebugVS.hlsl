#pragma pack_matrix(row_major)

cbuffer Model : register ( b0 )
{
	float4x4 modeling;
};

cbuffer Camera : register ( b1 )
{
	float4x4 view;
	float4x4 projection;
}

struct VERTEX
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};


OUTPUT main(VERTEX input)
{
	OUTPUT output = (OUTPUT)0;

	float4 temp = float4( input.pos.x, input.pos.y, input.pos.z, 1 );
	
	output.pos = mul(temp, modeling);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.color = input.color;

	return output;
}