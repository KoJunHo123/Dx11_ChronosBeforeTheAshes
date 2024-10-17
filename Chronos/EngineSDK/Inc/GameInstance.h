#pragma once

#include "Component_Manager.h"
#include "Renderer.h"
#include "PipeLine.h"

/* 1. 엔진과 클라이언트의 소통을 위한 클래스읻. */
/* 2. 엔진에서 클라이언트에 보여주고 싶은 함수들을 모아놓는다. */
/* 3. 함수들 -> 클래스의 멤버함수. 객체가 필요하다! 그래서 기능응ㄹ 제공해주기위한 클래스 객체를 보관한다. */
/* 4. 객체들을 보관하기위해 Initialize_Engine() 함수 안에서 기능을 제공해주기위한 클래스들을 객체화한다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Update_Engine(_float fTimeDelta);
	HRESULT Draw_Engine();
	HRESULT Clear(_uint iLevelIndex);

	_float Get_Random_Normal() const {
		return (_float)rand() / RAND_MAX;
	}

	_float Get_Random(_float fMin, _float fMax) const {
		return fMin + Get_Random_Normal() * (fMax - fMin);
	}

#pragma region GRAPHIC_DEVICE
public:
	void Render_Begin();
	void Render_End();
	ID3D11ShaderResourceView* Get_BackBuffer_SRV() const;
#pragma endregion

#pragma region INPUT_DEVICE
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);	
	_bool Get_DIKeyState_Down(_ubyte byKeyID);
	_bool Get_DIKeyState_Up(_ubyte byKeyID);
	_bool Get_DIMouseState_Down(MOUSEKEYSTATE eMouse);
	_bool Get_DIMouseState_Up(MOUSEKEYSTATE eMouse);
#pragma endregion

#pragma region LEVEL_MANAGER
	void Change_Level(class CLevel* pNextLevel);
#pragma endregion

#pragma region OBJECT_MANAGER
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex = 0);
	HRESULT Create_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void Release_Object(_uint iLevelIndex, const _wstring& strLayerTag);
	CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg);
	list<class CGameObject*> Get_GameObjects(_uint iLevelIndex, const _wstring& strLayerTag);
	size_t Get_ObjectSize(_uint iLevelIndex, const _wstring& strLayerTag);
	class CComponent* Find_PartComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);
	class CGameObject* Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iObjectIndex);
#pragma endregion


#pragma region COMPONENT_MANAGER
	HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr );
#pragma endregion

#pragma region TIMER_MANAGER
	HRESULT Add_Timer(const _wstring& strTimerTag);
	_float Compute_TimeDelta(const _wstring& strTimerTag);	
#pragma endregion

#pragma region RENDERER
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, class CGameObject* pRenderObject);
	HRESULT Bind_DefaultTexture(class CShader* pShader, const _char* pConstantName);
	_bool FadeIn(_float fTimeDelta);
	_bool FadeOut(_float fTimeDelta);

#ifdef _DEBUG
	HRESULT Add_DebugObject(class CComponent* pDebugObject);
#endif

#pragma endregion

#pragma region PIPELINE
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_matrix Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const;
	_float4 Get_CamPosition_Float4() const;
	_vector Get_CamPosition_Vector() const;
#pragma endregion

#pragma region PICKING
	_bool Picking(_float3* pPickPos);
#pragma endregion

#pragma region COLLISION_MANAGER
	void Add_CollisionKeys(const _wstring strCollisionFirst, const _wstring strCollisionSecond);
	void Add_Collider_OnLayers(const _wstring strCollisionKey, class CCollider* pCollider);
	void Erase_Collider(const _wstring strCollisionKey, class CCollider* pCollider);
#pragma endregion

#pragma region FONT_MANAGER
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float fRadian = 0.f, _fvector vPivot = XMVectorSet(0.f, 0.f, 0.f, 1.f), _float fScale = 1.f, _bool bCenterAligned = false);
#pragma endregion

#pragma region FRUSTUM
	_bool isIn_Frustum_WorldSpace(_fvector vPosition, _float fRadius = 0.f);
#pragma endregion

#pragma region TARGET_MANAGER
	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RT_ShaderResource(class CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName);
	HRESULT Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture);

#ifdef _DEBUG
public:
	HRESULT Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_MRT_Debug(const _wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	void Set_LightPos(_uint iIndex, _fvector vPos);
	void Set_LightDiffuse(_uint iIndex, _fvector vColor);
	void Set_LightRange(_uint iIndex, _float fRange);
#pragma endregion

#pragma region SOUND_MANAGER
	void SoundPlay(TCHAR* pSoundKey, _uint iChannelID, _float fVolume, _float3 SoundPos = {}, _float3 PlayerPos = {});
	void PlayBGM(TCHAR* pSoundKey, _uint iBGMChannel, _float fVolume);
	void StopSound(_uint iChannelID);
	void StopAll();
	void SetChannelVolume(_uint iChannelID, _float fVolume);
	void LoadSoundFile();
	void StopSoundSlowly(_uint iChannelID);
	_uint Get_SoundPosition(_uint iChannelID);
	void Set_SoundPosition(_uint iChannelID, _uint iPositionMS);
	_bool IsSoundPlaying(_uint iChannelID);
	void Set_SoundFrequency(_uint iChannelID, _float fFrequency);
	void CrossFade(_uint iSrcChannelID, _uint iDstChannelID, _float fMixSpeed, TCHAR* pDstSoundKey, _float3 SoundPos = {}, _float3 PlayerPos = {});
#pragma endregion

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CCollision_Manager*		m_pCollision_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CSound_Manager*			m_pSound_Manager = { nullptr };

public:	
	void Release_Engine();
	virtual void Free() override;
};

END