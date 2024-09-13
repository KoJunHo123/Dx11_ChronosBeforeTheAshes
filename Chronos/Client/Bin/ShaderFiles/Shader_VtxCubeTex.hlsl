#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;



struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
	
};

VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    vector vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    vPosition = mul(vPosition, g_ViewMatrix);
    vPosition = mul(vPosition, g_ProjMatrix);

    Out.vPosition = vPosition;
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
	
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}


technique11 DefaultTechnique
{
    pass Sky
    {
        SetRasterizerState(RS_Cull_CW); // 시계방향으로 찍힌(Clock Work) 폴리곤 제거.
        SetDepthStencilState(DSS_None, 0); // DSS_NONE : 깊이 비교 안함, 0 : 스텐실 비교도 안함
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        // BS_Default : 기본 블렌딩, 
        // (0,0,0,0) : 블렌딩 팩터. 블렌딩에 사용할 색상.
        // 0xffffffff : 어떤 색 블렌딩 할지. (RGBA)
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}