#include "Shader_Engine_Defines.hlsli"
/* float2 float3 float4 == vector */
/* float1x3, float3x3, float1x3, float4x4 == matrix */

/* VertexShader */
/* 정점하나당 어떤 변환을 거쳐가면 되는지를 직접 연산을 통해 구현. */
/* 정점 쉐이더는 정점 하나를 받아오고. 연산을 거친후 연산을 마친 정점을 리턴해준다. */
/* 사각형버퍼를 그린다. 6번호출. */
// VS_MAIN(/*정점*/float3 vPosition  : POSITION, float2 vTexcoord : TEXCOORD0)
//{
//
//}

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

float2			g_TexDivide;
int				g_iTexIndex;

float4          g_vColor = { 1.f, 1.f, 1.f, 1.f };

float           g_fRatio;

struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	

	/* 정점에 위치를 월드 뷰 투영변환한다.*/		
	/* 클라이언트에서 곱셈연산을 수행하는 TrnasformCoord함수와는 다르게 */
	/* mul함수의 경우에는 순수하게 행렬의 곱하기만 수행을 하고 w나누기연산자체는 수행하지 않는다. */
	vector		vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	vPosition = mul(vPosition, g_ViewMatrix);
	vPosition = mul(vPosition, g_ProjMatrix);

	Out.vPosition = vPosition;
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

/* Triangle : 정점 세개가 다 vs_main을 통과할때까지 대기 */
/* 세개가 모두다 통과되면. 밑의 과정을 수행. */
/* 리턴된 정점의 w로 정점의 xyzw를 나눈다. 투영 */
/* 정점의 위치를 뷰포트로 변환다. (윈도우좌표로 변환한다)*/
/* 래스터라이즈 : 정점정보를 기반으로하여 픽셀이 만들어진다. */

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;	
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);/*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/	

    if (Out.vColor.a < 0.1f)
        discard;
    
	return Out;
}

/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_BLENDUI_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    Out.vColor.a *= g_fRatio;
    
    return Out;
}


/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_COLOR_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/

    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor = g_vColor;
    
    return Out;
}

PS_OUT PS_SPRITE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord); /*vector(1.f, In.vTexcoord.y, 0.f, 1.f);*/
    
    if(Out.vColor.a < 0.1f)
        discard;
    //Out.vColor.rgb = g_vColor.rgb;
	
    return Out;
}

PS_OUT PS_FLASH_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
	
    Out.vColor.rgb = g_vColor.rgb * (1 - Out.vColor.a);
    
    return Out;
}

PS_OUT PS_SPARK_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    float2 start = (float2) 0;
    float2 over = (float2) 0;
	
    start.x = (1 / g_TexDivide.x) * g_iTexIndex;
    start.y = (1 / g_TexDivide.y) * (int) (g_iTexIndex / g_TexDivide.x);
	
    over.x = start.x + (1 / g_TexDivide.x);
    over.y = start.y + (1 / g_TexDivide.y);
	
    float2 vTexcoord = start + (over - start) * In.vTexcoord;
	
    Out.vColor = g_Texture.Sample(LinearSampler, vTexcoord);
	
    Out.vColor.a = Out.vColor.r;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    Out.vColor.rgb = g_vColor.rgb * (1 - Out.vColor.a);
    
    return Out;
}

PS_OUT PS_FLARE_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor.a = Out.vColor.r * saturate(g_vColor.a);
    
    Out.vColor.r = 1.f - (1 - g_vColor.r) * (1 - Out.vColor.a * 2.f);
    Out.vColor.g = 1.f - (1 - g_vColor.g) * (1 - Out.vColor.a * 2.f);
    Out.vColor.b = 1.f - (1 - g_vColor.b) * (1 - Out.vColor.a * 2.f);
        
    return Out;
}

technique11	DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
	pass UI
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass SPRITE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPRITE_MAIN();
    }

    pass FLASH
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FLASH_MAIN();
    }

    pass SPARK
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SPARK_MAIN();
    }

    pass FLARE
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_FLARE_MAIN();
    }

    pass BLENDUI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BLENDUI_MAIN();
    }

    pass COLORUI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_COLOR_MAIN();
    }



}
