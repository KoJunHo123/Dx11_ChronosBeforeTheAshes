#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CLabyrinth final : public CGameObject
{
private:
	CLabyrinth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLabyrinth(const CLabyrinth& Prototype);
	virtual ~CLabyrinth() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Add_Collider(_fvector vPos, _fvector vExtents);
	void Release_LastCollider();

	_uint Get_ColliderCount() {
		return m_ColliderComs.size();
	}

private:
	HRESULT Set_Rasterizer();

public:
	class CShader*				m_pShaderCom = { nullptr };	
	class CModel*				m_pModelCom = { nullptr };
	
	vector<class CCollider*>	m_ColliderComs;

	ID3D11RasterizerState*		m_pWireFrameRS = { nullptr };
	ID3D11RasterizerState*		m_pSolidFrameRS = { nullptr };

	_bool m_bRenderSkip = { false };

private:
	HRESULT Ready_Components();

public:
	static CLabyrinth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END