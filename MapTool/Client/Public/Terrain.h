#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_vector vPos;
	}TERRAIN_DESC;

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& Prototype);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	class CShader*				m_pShaderCom = { nullptr };
	class CTexture*				m_pTextureCom = { nullptr };
	class CVIBuffer_Terrain*	m_pVIBufferCom = { nullptr };
	class CNavigation*			m_pNavigationCom = { nullptr };

	ID3D11RasterizerState* m_pWireFrameRS = { nullptr };
	ID3D11RasterizerState* m_pSolidFrameRS = { nullptr };

	
private:
	HRESULT Ready_Components();
private:
	HRESULT Set_Rasterizer();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END