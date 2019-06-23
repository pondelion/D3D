uniform extern float4x4 gWVP;
uniform extern float time;

struct OutVS
{
	float4 pos	: POSITION0;
};

OutVS gridVS(float3 posL : POSITION0)
{
	float y=0.0f;
	int i,j,n=10,m=10;
	OutVS myout = (OutVS)0;
	n = m = (int)(3.0f*(cos(time)+1));
	for(i=1;i<=n;i++)
	{
		for(j=1;j<=m;j++)
		{
		y += 10.0f*(1.0f-cos((float)i*3.14159f/2.0f))*(1.0f-cos((float)j*3.14159f/2.0f))*sin((float)i*3.14159f*(posL.x+5.0f)/10.0f)*sin((float)j*3.14159f*(posL.z+5.0f)/10.0f)/((float)i*(float)j*3.14159f*3.14159f);;
		
		}
	}
	
	
	
	
	posL.y = y;
	myout.pos = mul(float4(posL, 1.0f), gWVP);
	
	return myout;
}

float4 gridPS() : COLOR
{
	//return float4(0.0,0.0,0.0,1.0);
	return float4(abs(cos(2.0f*time)), abs(sin(time)), abs(sin(time)*cos(time)), 1.0f);
}

technique gridTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 gridVS();
		pixelShader  = compile ps_2_0 gridPS();
		
		FillMode = Wireframe;
	}
}
