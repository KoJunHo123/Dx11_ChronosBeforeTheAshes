#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_DiffuseTexture;


struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;		
	float3 vTangent : TANGENT;
};

struct VS_OUT
{	
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
};

VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

RasterizerState rsWireframe
{
    FillMode = WireFrame;
};


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	// 최종 픽셀을 결정하는 옵션 필터.
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

	if(Out.vColor.a < 0.3f)
        discard;
	
	return Out;
}

technique11	DefaultTechnique
{
	pass Model
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        // SetRasterizerState(rsWireframe);
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}