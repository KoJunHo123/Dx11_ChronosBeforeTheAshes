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

class CUI_PuzzleExit final : public CBlend_UI_Object
{
public:
	typedef struct : CBlend_UI_Object::UI_DESC
	{
		class CPlayer* pPlayer = { nullptr };
	}UI_PUZZLEEXIT_DESC;

private:
	CUI_PuzzleExit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_PuzzleExit(const CUI_PuzzleExit& Prototype);
	virtual ~CUI_PuzzleExit() = default;

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
	class CCamera* m_pCurrentCamera = { nullptr };

	_bool m_bOn = { false };

private:
	HRESULT Ready_Components();

public:
	static CUI_PuzzleExit* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};

END