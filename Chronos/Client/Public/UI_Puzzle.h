#pragma once

#include "Client_Defines.h"
#include "Blend_UI_Object.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CCamera;
END

BEGIN(Client)

class CUI_Puzzle final : public CBlend_UI_Object
{
public:
	typedef struct : CBlend_UI_Object::UI_DESC
	{
		class CPlayer* pPlayer = { nullptr };
		class CInventory* pInventory = { nullptr };
	}UI_PUZZLE_DESC;

private:
	CUI_Puzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Puzzle(const CUI_Puzzle& Prototype);
	virtual ~CUI_Puzzle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CPlayer* m_pPlayer = { nullptr };
	class CInventory* m_pInventory = { nullptr };
	class CCamera* m_pCurrentCamera = { nullptr };

	_float m_fRatio = { 0.f };

	_bool m_bOn = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_Puzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};

END