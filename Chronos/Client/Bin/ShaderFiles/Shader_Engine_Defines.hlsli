

// 텍스처 샘플링
// 포인트 샘플링
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

// 선형 샘플링
sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;    // 좌표 벗어나는 경우 0부터 다시 시작.
    AddressV = wrap;    
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;   // 좌표를 벗어나는 경우 가장 가가운 범위 내의 값으로 제한.(0 보다 밑에 있으면 0으로, 1보다 위에 있으면 1로.)
    AddressV = clamp;
};

// 레스터라이저 : 3D 그래픽을 그리기 위해 벡터 정보를 픽셀 이미지로 변환. == 3차원 삼각형으로부터 픽셀 색상들을 계산. -> 이걸 하고나서 픽셀 셰이더.
RasterizerState RS_Default  
{
    FillMode = solid;   // 삼각형 안을 어떻게 채울 거냐
    CullMode = back;    // 어느 부분을 아예 날릴거냐.
    FrontCounterClockwise = false;  // 반시계 방향을 앞으로 바꿀래?
};

RasterizerState RS_Wireframe
{
    FillMode = wireframe;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = solid;
    CullMode = none;
    FrontCounterClockwise = false;
};

// 깊이 스텐실 상태
DepthStencilState DSS_Default
{
    DepthEnable = true;     // 깊이 비교 할래 말래
    DepthWriteMask = all;   // 깊이값 기록 할래 말래
    DepthFunc = less_equal; // 깊이값 기록할 때 조건 좀 더 세세하게. (less_equal == 작거나 같을 떄)
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

// 블렌딩 상태
BlendState BS_Default
{
    BlendEnable[0] = false; // 뭐 안할게요.
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;  // 할거예요. 왜 배열인지는 아직 몰루...

    SrcBlend = Src_Alpha;   // 섞을 대상 = Src의 알파
    DestBlend = Inv_Src_Alpha;  // 섞일 대상 = 1 - Src의 알파.
    BlendOp = Add;  // 더하기로.

	/*

	SrcBlend[0] = Src_Alpha;
	DestBlend[0] = Inv_Src_Alpha;
	BlendOp[0] = Add;

	SrcBlend[1] = one;
	DestBlend[1] = one;
	BlendOp[1] = Add;*/
};

// 블렌딩을 걍 50:50으로 -> 알파값을 걍 0.5라 침.
BlendState BS_OnebyOne
{
    BlendEnable[0] = true;

    SrcBlend = one;
    DestBlend = one;
    BlendOp = Add;
};