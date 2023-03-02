
struct Output
{
	float4 vPosition : SV_POSITION;
	float2 vUV : TEXCOORD;
};

Output main(uint ID : SV_VertexID)
{
	Output output;

	switch(ID)
	{
		case 0:  //TL
		{
			output.vPosition = float4( -1.0f, 1.0f, 0.0f, 1.0f );
			output.vUV = float2(0, 0);

			break;
		}

		case 1: //TR
		case 3:
		{
			output.vPosition = float4( 1.0f, 1.0f, 0.0f, 1.0f );
			output.vUV = float2( 1, 0 );

			break;
		}

		case 2: //BL
		case 5:	
		{
			output.vPosition = float4( -1.0f, -1.0f, 0.0f, 1.0f );
			output.vUV = float2( 0, 1 );

			break;
		}

		case 4: //BR
		{
			output.vPosition = float4( 1.0f, -1.0f, 0.0f, 1.0f );
			output.vUV = float2( 1, 1 );

			break;
		}
	}



	return output;
}