#include "Shader_Engine_Defines.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
vector g_vCamPosition;

float4 g_vColor;
float g_fRatio;
float g_fScale;

struct VS_IN
{
	/* InputSlot : 0 */
    float3 vPosition : POSITION;
	/* InputSlot : 1 */ 
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN( /*정점*/VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    Out.vPosition = float4(In.vCurPos, 1.f);
    Out.vCurPos = In.vCurPos;
    Out.vPrePos = In.vPrePos;
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct GS_IN
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
    float4 vPosition : POSITION;
    float3 vCurPos : TEXCOORD0;
    float3 vPrePos : TEXCOORD1;
    float2 vLifeTime : TEXCOORD2;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

[maxvertexcount(6)] // 꼭 해줘야 함. 점을 몇 번 찍을 건지.(인덱스 갯수)
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
    GS_OUT Out[4];

	// 빌보드용.
    float3 vLook = normalize((g_vCamPosition - In[0].vPosition).xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook));
    float3 vUp = normalize(cross(vLook, vRight));

    float fLifeTime = 1 - (In[0].vLifeTime.y / In[0].vLifeTime.x) + 0.05f;
    
    Out[0].vPosition = float4(In[0].vPrePos + vUp * g_fScale * fLifeTime, 1.f);
    Out[0].vTexcoord = float2(0.f, 0.f);
    Out[0].vLifeTime = In[0].vLifeTime;

    Out[1].vPosition = float4(In[0].vCurPos + vUp * g_fScale * fLifeTime, 1.f);
    Out[1].vTexcoord = float2(1.f, 0.f);
    Out[1].vLifeTime = In[0].vLifeTime;

    Out[2].vPosition = float4(In[0].vCurPos - vUp * g_fScale * fLifeTime, 1.f);
    Out[2].vTexcoord = float2(1.f, 1.f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(In[0].vPrePos - vUp * g_fScale * fLifeTime, 1.f);
    Out[3].vTexcoord = float2(0.f, 1.f);
    Out[3].vLifeTime = In[0].vLifeTime;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);

    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);

    Container.Append(Out[0]);
    Container.Append(Out[1]);
    Container.Append(Out[2]);
    Container.RestartStrip(); // 여기서부터 다시 찍겠다. 이거 안해주면 123으로 찍어버림.

    Container.Append(Out[0]);
    Container.Append(Out[2]);
    Container.Append(Out[3]);
    Container.RestartStrip();
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_TARGET0;
};


/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vColor = Out.vColor;
    
    if (Out.vColor.a < 0.1f)
        discard;
    
    if (In.vLifeTime.x < In.vLifeTime.y)
        discard;
    
    Out.vColor.rgb = 1.f - (1 - g_vColor.rgb) * (1 - vColor.a * 0.85f);
    
    return Out;
}



technique11 DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
    pass UI
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}