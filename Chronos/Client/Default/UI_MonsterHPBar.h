#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CUI_MonsterHPBar final : public CUIObject
{
public:
	typedef struct : CUIObject::UI_DESC
	{
		class CMonster* pMonster = { nullptr };
	}UI_HPBAR_DESC;

private:
	CUI_MonsterHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_MonsterHPBar(const CUI_MonsterHPBar& Prototype);
	virtual ~CUI_MonsterHPBar() = default;


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };

private:
	class CMonster* m_pMonster = { nullptr };
	_float4 m_vColor = {};

private:
	_float m_fHP = { 0.f };
	_float m_fMaxHP = { 0.f };
	_float m_fRatio = { 0.f };

	_float m_fZ = { 0.f };

private:
	HRESULT Ready_Components();

public:
	static CUI_MonsterHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;

};
END
