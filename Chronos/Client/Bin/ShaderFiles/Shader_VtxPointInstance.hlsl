#include "Shader_Engine_Defines.hlsli"

matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;
vector			g_vCamPosition;

struct VS_IN
{
	/* InputSlot : 0 */
	float3 vPosition : POSITION;	
	float2 vPSize : PSIZE;
	/* InputSlot : 1 */ 
    row_major float4x4 TransformMatrix : WORLD; //row_majo : 행을 기준으로.
	float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct VS_OUT
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : COLOR0;	
    float4 vColor : COLOR1;
};

/* 1. 정점의 변환과정을 수행한다. */
/* 2. 정점의 구성 정보를 변형한다. */
VS_OUT VS_MAIN(/*정점*/VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;	
	
	/* 로컬상태에서의 움직임 구현. */
	vector		vPosition = mul(float4(In.vPosition, 1.f), In.TransformMatrix);

	/* 월드에 특정 상태로 배치한다. */
	vPosition = mul(vPosition, g_WorldMatrix);
	
	Out.vPosition = vPosition;
	Out.vPSize = In.vPSize;
	Out.vLifeTime = In.vLifeTime;
    Out.vColor = In.vColor;
	
	return Out;
}

struct GS_IN
{
	/* SV_ : ShaderValue */
	/* 내가 해야할 연산은 모두 마쳐놓은 것이므로 이후 dx가 추가적으로 해야할 이릉ㄹ 해라. */
	float4 vPosition : POSITION;
	float2 vPSize : PSIZE;
	float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct GS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexcoord : TEXCOORD0;
	float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

// void GS_MAIN(triangle GS_IN In[3], )
// In을 넣으면 inout TriangleStream<GS_OUT> Container 으로 나옴.
// 포인트니까 In[1]. 라인이면 2, 트라이앵글이면 3.
[maxvertexcount(6)]	// 꼭 해줘야 함. 점을 몇 번 찍을 건지.(인덱스 갯수)
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Container)
{
	GS_OUT			Out[4];

	// 빌보드용.
	float3		vLook = (g_vCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook)) * In[0].vPSize.x * 0.5f;
	float3		vUp = normalize(cross(vLook, vRight)) * In[0].vPSize.y * 0.5f;

	Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.f);
	Out[0].vTexcoord = float2(0.f, 0.f);
	Out[0].vLifeTime = In[0].vLifeTime;
    Out[0].vColor = In[0].vColor;

	Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.f);
	Out[1].vTexcoord = float2(1.f, 0.f);
	Out[1].vLifeTime = In[0].vLifeTime;
    Out[1].vColor = In[0].vColor;

	Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.f);
	Out[2].vTexcoord = float2(1.f, 1.f);
	Out[2].vLifeTime = In[0].vLifeTime;
    Out[2].vColor = In[0].vColor;

	Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.f);
	Out[3].vTexcoord = float2(0.f, 1.f);
	Out[3].vLifeTime = In[0].vLifeTime;
    Out[3].vColor = In[0].vColor;

	matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[3].vPosition = mul(Out[3].vPosition, matVP);

	Container.Append(Out[0]);
	Container.Append(Out[1]);
	Container.Append(Out[2]);
	Container.RestartStrip();	// 여기서부터 다시 찍겠다. 이거 안해주면 123으로 찍어버림.

	Container.Append(Out[0]);
	Container.Append(Out[2]);
	Container.Append(Out[3]);
	Container.RestartStrip();
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
	float2 vLifeTime : COLOR0;
    float4 vColor : COLOR1;
};

struct PS_OUT
{
	vector vColor : SV_TARGET0;
};


/* 1. 픽셀의 최종적인 색상을 결정한다. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;	
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a <= 0.3f)
		discard;

	Out.vColor.rgb = In.vColor.rgb;
	Out.vColor.a = Out.vColor.a * (1.f - (In.vLifeTime.y / In.vLifeTime.x));	//1~0

	if (In.vLifeTime.y >= In.vLifeTime.x)	// 1보다 커지면 꺼.
		discard;

	return Out;
}

PS_OUT PS_SPAWN_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT) 0;
	
	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	if (Out.vColor.a <= 0.3f)
		discard;
	
    Out.vColor.r = In.vColor.r * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));
    Out.vColor.g = In.vColor.g * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));
    Out.vColor.b = In.vColor.b * (1.f - ((In.vLifeTime.y / In.vLifeTime.x) * 0.25f));

	return Out;
}

technique11	DefaultTechnique
{
	/* 빛연산 + 림라이트 + ssao + 노멀맵핑 + pbr*/
	pass UI
	{
		SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass SPAWN
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SPAWN_MAIN();
    }
	/* 디스토션 + 블렌딩 */
	//pass Effect
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN_Special();
	//	PixelShader = compile ps_5_0 PS_MAIN_Special();
	//}
}