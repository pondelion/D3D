
uniform extern float4x4 gW;
uniform extern float4x4 gWIT;
uniform extern float4x4 gWVP;
uniform extern float3	gCamPos;
uniform extern float4	gAmbMtrl;
uniform extern float4	gDifMtrl;
uniform extern float4	gSpecMtrl;
uniform extern float	gPower;
uniform extern float3	gLightPos;
uniform extern float4	gAmbLight;
uniform extern float4	gDifLight;
uniform extern float4	gSpecLight;
uniform extern float3	gAttenuation;
uniform extern float	gTime;

struct OutVS
{
	float4	pos		: POSITION0;
	float4	color	: COLOR0;
};

OutVS PLVS(float3 posL : POSITION0, float3 normalL : NORMAL0)
{
	OutVS	myout = (OutVS)0;
	
	float3	normalW = mul(float4(normalL, 0.0f), gWIT).xyz;
	normalW = normalize(normalW);
	
	float3 posW = mul(float4(posL, 1.0f), gW).xyz;
	
	float3 lvW = normalize(gLightPos-posW);
	
	float3 ambient = (gAmbMtrl*gAmbLight).rgb;
	
	float	dif = max(dot(normalW, lvW), 0.0f);
	float3	diffuse = dif*(gDifMtrl*gDifLight).rgb;
	
	float3	VtoCamW	= normalize(gCamPos-posW);
	float3	ref	= reflect(-lvW, normalW);
	float	sp = pow(max(dot(ref, VtoCamW), 0.0f), gPower);
	float3	spec = sp*(gSpecMtrl*gSpecLight).rgb;
	
	float	d = distance(gLightPos,posW);
	float	attenu = gAttenuation.x + gAttenuation.y*d + gAttenuation.z*d*d;
	
	float3 color = ambient + (diffuse+spec)/attenu;
	
	myout.color = float4(color, gDifMtrl.a);
	
	myout.pos = mul(float4(posL, 1.0f), gWVP);
	
	return myout;
}

float4	PLPS(float4 col : COLOR) : COLOR
{
	return col;

}

technique PLTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 PLVS();
		pixelShader	 = compile ps_2_0 PLPS();
	}
}
