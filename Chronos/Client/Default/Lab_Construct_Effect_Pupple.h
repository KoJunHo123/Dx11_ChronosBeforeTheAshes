#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Point_Instance;
class CTexture;
END

BEGIN(Client)
class CLab_Construct_Effect_Pupple final : public CPartObject
{
public:
	typedef struct : CPartObject::PARTOBJ_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	}EFFECT_DESC;

private:
	CLab_Construct_Effect_Pupple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Construct_Effect_Pupple(const CLab_Construct_Effect_Pupple& Prototype);
	virtual ~CLab_Construct_Effect_Pupple() = default;

public:
	void Set_On(_bool bOn) {
		if (m_bOn != bOn)
			m_bOn = bOn;

		if (true == m_bOn)
			m_bOver = false;
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	class CTexture* m_pTextureCom = { nullptr };
	class CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	const _float4x4* m_pSocketMatrix = { nullptr };
	_float2 m_vDivide = {};
	_float4 m_vColor = {};

	_bool m_bOn = { false };
	_bool m_bOver = { false };

private:
	HRESULT Ready_Components();

public:
	static CLab_Construct_Effect_Pupple* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CLab_Construct_Effect_Pupple* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free();
};
END