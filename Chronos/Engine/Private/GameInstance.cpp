#include "..\Public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Input_Device.h"
#include "Picking.h"
#include "Collision_Manager.h"
#include "Font_Manager.h"
#include "Culling.h"
#include "Target_Manager.h"
#include "Light_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 카드를 초기화하낟. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.isWindowsed, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pCulling = CCulling::Create(*ppDevice, *ppContext);
	if (nullptr == m_pCulling)
		return E_FAIL;

	/* 사운드 카드를 초기화하낟. */

	/* 입력장치를 초기화하낟. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	/* 여러가지 매니져를 초기화하낟. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pCollision_Manager = CCollision_Manager::Create();
	if (nullptr == m_pCollision_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return S_OK;
}



void CGameInstance::Update_Engine(_float fTimeDelta)
{
	/* 현재 키보드와 마우스의 상태를 받아올꺼야. */
	m_pInput_Device->Update();

	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pPipeLine->Update();

	m_pCulling->Culling_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pCollision_Manager->Update();

	m_pObject_Manager->Late_Update(fTimeDelta);

	m_pLevel_Manager->Update(fTimeDelta);
}

HRESULT CGameInstance::Draw_Engine()
{
	m_pRenderer->Draw();

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Clear(_uint iLevelIndex)
{
	/* 특정 레벨을 위해 존재하는 자원들은 해당 레벨이 끝나면 삭제하자. */
	m_pObject_Manager->Clear(iLevelIndex);

	/* 컴포넌트 원형들도 레벨별로 관리했었다. */
	// m_pComponent_Manager->Clear(iLevelIndex);

	return S_OK;
}

void CGameInstance::Render_Begin()
{
	/*m_pGraphic_Device->Render_Begin();*/
	m_pGraphic_Device->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGraphic_Device->Clear_DepthStencil_View();


}

void CGameInstance::Render_End()
{
	/*m_pGraphic_Device->Render_End(hWnd);*/

	m_pGraphic_Device->Present();


}
#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

_bool CGameInstance::Get_DIKeyState_Down(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Down(byKeyID);
}

_bool CGameInstance::Get_DIKeyState_Up(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState_Up(byKeyID);
}

_bool CGameInstance::Get_DIMouseState_Down(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Down(eMouse);
}

_bool CGameInstance::Get_DIMouseState_Up(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState_Up(eMouse);
}

#pragma endregion

#pragma region LEVEL_MANAGER

void CGameInstance::Change_Level(CLevel* pNextLevel)
{
	return m_pLevel_Manager->Change_Level(pNextLevel);
}

#pragma endregion

#pragma region OBJECT_MANAGER

HRESULT CGameInstance::Add_Prototype(const _wstring& strPrototypeTag, CGameObject* pPrototype)
{
	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}


HRESULT CGameInstance::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Add_CloneObject_ToLayer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CComponent* CGameInstance::Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	return m_pObject_Manager->Find_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

HRESULT CGameInstance::Create_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Create_Layer(iLevelIndex, strLayerTag);
}
void CGameInstance::Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	m_pObject_Manager->Clear_Layer(iLevelIndex, strLayerTag);
}
void CGameInstance::Release_Object(_uint iLevelIndex, const _wstring& strLayerTag)
{
	m_pObject_Manager->Release_Object(iLevelIndex, strLayerTag);
}
CGameObject* CGameInstance::Clone_GameObject(const _wstring& strPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}
list<class CGameObject*> CGameInstance::Get_GameObjects(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_GameObjects(iLevelIndex, strLayerTag);
}
size_t CGameInstance::Get_ObjectSize(_uint iLevelIndex, const _wstring& strLayerTag)
{
	return m_pObject_Manager->Get_ObjectSize(iLevelIndex, strLayerTag);
}
CComponent* CGameInstance::Find_PartComponent(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	return m_pObject_Manager->Find_PartComponent(iLevelIndex, strLayerTag, strComponentTag, iIndex, iPartObjIndex);
}
CGameObject* CGameInstance::Get_GameObject(_uint iLevelIndex, const _wstring& strLayerTag, _uint iObjectIndex)
{
	return m_pObject_Manager->Get_GameObject(iLevelIndex, strLayerTag, iObjectIndex);
}
#pragma endregion

#pragma region COMPONENT_MANAGER

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CComponent* pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

#pragma endregion

#pragma region TIMER_MANAGER

HRESULT CGameInstance::Add_Timer(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimerTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

#pragma endregion


#pragma region RENDERER

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroupID, CGameObject* pRenderObject)
{
	return m_pRenderer->Add_RenderObject(eRenderGroupID, pRenderObject);
}
HRESULT CGameInstance::Bind_DefaultTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pRenderer->Bind_DefaultTexture(pShader, pConstantName);
}
_bool CGameInstance::FadeIn(_float fTimeDelta)
{
	return m_pRenderer->FadeIn(fTimeDelta);
}
_bool CGameInstance::FadeOut(_float fTimeDelta)
{
	return m_pRenderer->FadeOut(fTimeDelta);
}
#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugObject(CComponent* pDebugObject)
{
	return m_pRenderer->Add_DebugObject(pDebugObject);
}
#endif
#pragma endregion

#pragma region PIPELINE
void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}
_float4x4 CGameInstance::Get_Transform_Inverse_Float4x4(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Float4x4(eState);
}
_matrix CGameInstance::Get_Transform_Inverse_Matrix(CPipeLine::D3DTRANSFORMSTATE eState) const
{
	return m_pPipeLine->Get_Transform_Inverse_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	return m_pPipeLine->Get_CamPosition_Float4();
}
_vector CGameInstance::Get_CamPosition_Vector() const
{
	return m_pPipeLine->Get_CamPosition_Vector();
}
#pragma endregion

#pragma region PICKING
_bool CGameInstance::Picking(_float3* pPickPos)
{
	return m_pPicking->Picking(pPickPos);
}
#pragma endregion


#pragma region COLLISION_MANAGER
void CGameInstance::Add_CollisionKeys(const _wstring strCollisionFirst, const _wstring strCollisionSecond)
{
	if (nullptr == m_pCollision_Manager)
		return;
	return m_pCollision_Manager->Add_CollisionKeys(strCollisionFirst, strCollisionSecond);
}
void CGameInstance::Add_Collider_OnLayers(const _wstring strCollisionKey, CCollider* pCollider)
{
	if (nullptr == m_pCollision_Manager)
		return;
	return m_pCollision_Manager->Add_Collider_OnLayers(strCollisionKey, pCollider);
}
void CGameInstance::Erase_Collider(const _wstring strCollisionKey, CCollider* pCollider)
{
	if (nullptr == m_pCollision_Manager)
		return;
	return m_pCollision_Manager->Erase_Collider(strCollisionKey, pCollider);
}
#pragma endregion

#pragma region FONT_MANAGER 

HRESULT CGameInstance::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, pFontFilePath);
}
HRESULT CGameInstance::Render_Text(const _wstring& strFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vPivot, _float fScale, _bool bCenterAligned)
{
	return m_pFont_Manager->Render(strFontTag, pText, vPosition, vColor, fRadian, vPivot, fScale, bCenterAligned);
}
#pragma endregion

#pragma region CULLING
void CGameInstance::Culling_Update(_float fDeltaTime)
{
	return m_pCulling->Culling_Update(fDeltaTime);
}

HRESULT CGameInstance::is_Culling(CTransform* pTransform)
{
	return m_pCulling->is_Culling(pTransform);
}
#pragma endregion

#pragma region TARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}
HRESULT CGameInstance::Bind_RT_ShaderResource(CShader* pShader, const _wstring& strTargetTag, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_RenderTarget(const _wstring& strTargetTag, ID3D11Texture2D* pTexture)
{
	return m_pTarget_Manager->Copy_RenderTarget(strTargetTag, pTexture);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RT_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Render_MRT_Debug(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render(strMRTTag, pShader, pVIBuffer);
}
#endif
#pragma endregion

#pragma region LIGHT_MANAGER
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(LightDesc);
}
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pCulling);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);

	CGameInstance::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	__super::Free();

}
