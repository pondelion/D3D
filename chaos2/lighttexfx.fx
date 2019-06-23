uniform	extern float4x4 gW;
uniform extern float4x4 gWIT;
uniform extern float4x4 gWVP;
uniform extern float4	gAmbMtrl;
uniform extern float4	gAmbLight;
uniform extern float4	gDifMtrl;
uniform extern float4	gDifLight;
uniform extern float4	gSpecMtrl;
uniform extern float4	gSpecLight;
uniform extern float3	gVLight;
uniform extern float3	gCamPos;
uniform extern float	gPower;
uniform	extern texture	gTex;
uniform extern float	gTime;

sampler SampleTex = sampler_state
{
	Texture = <gTex>;
	MinFilter = Anisotropic;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	MaxAnisotropy = 8;
	AddressU	= WRAP;
	AddressV	= WRAP;
};

struct OutVS
{
	float4 pos		: POSITION0;
	float4 diffuse	: COLOR0;
	float4 spec		: COLOR1;
	float2 tex		: TEXCOORD0;
};

OutVS LightTexVS(float3 posL : POSITION0, float3 nL : NORMAL0, float2 tex : TEXCOORD0)
{
	OutVS myout = (OutVS)0;
	
	float3 nW = normalize(mul(float4(nL, 0.0f), gWIT).xyz);
	
	float3 posW = mul(float4(posL, 1.0f), gW).xyz;
	
	float3 PtoCam = normalize(gCamPos-posW);
	
	float3 ref = reflect(-gVLight, nW);
	
	float s = pow(max(dot(ref,PtoCam), 0.0f), gPower);
	
	float d = max(dot(gVLight, nW), 0.0f);
	
	float3 spec = s*(gSpecMtrl*gSpecLight).rgb;
	float3 diffuse = d*(gDifMtrl*gDifLight).rgb;
	float3 ambient = gAmbMtrl*gAmbLight;
	
	myout.pos = mul(float4(posL, 1.0f), gWVP);
	myout.diffuse = float4(ambient+diffuse, gDifMtrl.a);
	myout.spec	= float4(spec, 0.0f);
	myout.tex	= tex;
	
	return myout;
}

float4 LightTexPS(float4 dif : COLOR0, float4 spec : COLOR1, float2 tex : TEXCOORD0) : COLOR
{
	float4 t = tex2D(SampleTex, tex);
	return float4( dif.rgb * t.rgb + spec.rgb, t.a*dif.a);//+float4(0.0f, abs((1.0f+sin(2.0f*gTime))*0.5f), 0.0f, 0.0f);
}

technique LightTexTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 LightTexVS();
		pixelShader	 = compile ps_2_0 LightTexPS();
	}
}
