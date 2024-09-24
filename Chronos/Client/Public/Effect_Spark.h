#pragma once

#include "BlendObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Spark final : public CBlendObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float3 vPos;
		_float3 vPivot;
		_float4 vColor;
	}SPARK_DESC;
private:
	CEffect_Spark(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Spark(const CEffect_Spark& Prototype);
	virtual ~CEffect_Spark() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CTexture* m_pTextureCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	_float4 m_vColor = {};

	_float2 m_vDivide = { };
	_float m_fTexIndex = { 0 };

private:
	HRESULT Ready_Components();
	HRESULT Add_SparkParticle(_fvector vPos, _fvector vPivot);

public:
	static CEffect_Spark* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
