#define MAX_LIGHTS 16

Texture2D diffuseMap  : register( t0 );
Texture2D normalMap   : register( t1 );
Texture2D specularMap : register( t2 );

SamplerState filter	: register( s0 );

struct TPointLight
{
	float4 vLightDesc;
	float4 vColor;
};

cbuffer LightsBuffer : register( b0 )
{
	float3	vBufferData;
	float	fDissolve;
	float3	vCameraPos;
	float	fPadding;
	TPointLight	tLights[MAX_LIGHTS];
}

cbuffer TextureBuffer : register ( b1 )
{
	float2 vScreenResolution;
	bool bHasTexture;
	bool bHasNormalMap;
	bool bHasSpecular;
	float fMaskingValue;
	float fOutlineValue;
	float fTime;
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

float4 CalculatePointLight(float3 vSurfacePos, float3 vSurfaceNormal, int nIndex)
{
	float3 vPixelToLight = tLights[nIndex].vLightDesc.xyz - vSurfacePos;

	float fDistanceToLight = length(vPixelToLight);

	float fAngularAttenuation = saturate(dot(vPixelToLight / fDistanceToLight, vSurfaceNormal));

	float fRangeAttenuation = 1 - saturate(fDistanceToLight / tLights[nIndex].vLightDesc.w);

	return fAngularAttenuation * tLights[nIndex].vColor * fRangeAttenuation * fRangeAttenuation;
}

float3 CalculateNormal(INPUT input)
{
	float3 vBumpmap = normalMap.Sample(filter, input.vUV).xyz * 2 - 1.0f;
	float3 vTangent = input.vTangent - dot(input.vTangent, input.vNormal) * input.vNormal;
	float3 vBitangent = cross(input.vNormal, vTangent);

	float3x3 TBN = float3x3 ( vTangent, vBitangent, input.vNormal );

	return normalize(mul(vBumpmap, TBN));
}

float4 main(INPUT input) : SV_TARGET
{
	float4 vBaseColor = float4(0,0,0,0);

	if(bHasTexture)
		vBaseColor = diffuseMap.Sample(filter, input.vUV);

	if(bHasNormalMap)
		input.vNormal = CalculateNormal(input);
	
	float4 vLightsColor = float4( 0, 0, 0, 1 );

	float3 vCamToObj = normalize(input.vWorldPos - vCameraPos);
	float4 vSpecularValue = float4( 0, 0, 0, 0 );

	for(int i = 0; i < vBufferData.x; i++)
	{
		vLightsColor += CalculatePointLight(input.vWorldPos, input.vNormal, i);

		float3 vLightToObj = input.vWorldPos - tLights[i].vLightDesc.xyz;
		float3 vReflection = reflect(normalize(vLightToObj), input.vNormal);
		if(bHasSpecular)
		{
			float fSpecularFactor = pow(saturate(dot(vCamToObj, -vReflection)), 150);
			float fRangeAttenuation = 1 - saturate(length(vLightToObj) / tLights[i].vLightDesc.w);
			vSpecularValue += specularMap.Sample(filter, input.vUV) * fSpecularFactor * fRangeAttenuation * tLights[i].vColor;
		}
	}
	
	return ( vBaseColor * saturate(vLightsColor) ) + vSpecularValue + vBaseColor * vBufferData.y;
}