#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D		g_DiffuseTexture;
texture2D       g_ComboTexture;
texture2D       g_EmissiveTexture;
texture2D		g_NoiseTexture;
float			g_fRatio;
float4          g_vEmissiveColor;

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
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(/*Á¤Á¡*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
    Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;

	return Out;
}

RasterizerState rsWireframe
{
    FillMode = WireFrame;
};


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
    vector vCombo : SV_TARGET3;
    vector vPickDepth : SV_TARGET4;
    vector vEmissive : SV_TARGET5;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vMtrlDiffuse.a)
        discard;
    
    vector vMtrlComb = g_ComboTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vCombo = vMtrlComb;
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vEmissive = vMtrlEmissive * g_vEmissiveColor;

    return Out;
}

PS_OUT PS_MAIN_WEAPON(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (0.3f >= vMtrlDiffuse.a)
        discard;
    
    vector vMtrlNoise = g_NoiseTexture.Sample(LinearSampler, In.vTexcoord);
	
    if (g_fRatio > vMtrlNoise.r)
        discard;
    
    vector vMtrlComb = g_ComboTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlEmissive = g_EmissiveTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);
    Out.vCombo = vMtrlComb;
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, 0.f, 0.f, 1.f);
    Out.vEmissive = vMtrlEmissive * g_vEmissiveColor;

    return Out;
}

struct PS_OUT_LIGHTDEPTH
{
    vector vLightDepth : SV_TARGET0;
};

PS_OUT_LIGHTDEPTH PS_MAIN_LIGHTDEPTH(PS_IN In)
{
    PS_OUT_LIGHTDEPTH Out = (PS_OUT_LIGHTDEPTH) 0;

    Out.vLightDepth = vector(In.vProjPos.w / 1000.f, 0.f, 0.f, 0.f);

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
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Model_Weapon
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_WEAPON();
    }

    pass LightDepth
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_LIGHTDEPTH();
    }

}