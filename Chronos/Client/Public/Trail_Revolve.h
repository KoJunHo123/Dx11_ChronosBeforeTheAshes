#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Trail_Instance;
END

BEGIN(Client)
class CTrail_Revolve final : public CGameObject
{
public:
	enum TYPE { TYPE_SPREAD, TYPE_CONSTANT, TYPE_CONVERGE, TYPE_END };

public:
	typedef struct
	{
		_uint iTrailCount = { 0 };
		_float4 vColor = {};
		_float3 vPos = {};
		_float3 vRange = {};
		_float fAccel = { 0.f };
		_float fSpeed = { 0.f };
		_float fScale = { 0.f };
		_float fRotaionPerSecond = { 0.f };
		_float fTypeAccel = { 0.f };
		TYPE eType = { TYPE_CONSTANT };

		_float fTime = { 0.f };
	}TRAIL_REVOLOVE_DESC;

private:
	CTrail_Revolve(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Revolve(const CTrail_Revolve& Prototype);
	virtual ~CTrail_Revolve() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	class CShader* m_pShaderCom = { nullptr };
	vector<class CVIBuffer_Trail_Instance*> m_VIBuffers;
	class CTexture* m_pTextureCom = { nullptr };

	_float4 m_vColor = {};
	vector<_float3> m_MovePoses = {};
	_float m_fRotaionPerSecond = { 0.f };
	_float m_fScale = { 0.f };
	_float m_fSpeed = { 0.f };
	_float m_fAccel = { 0.f };
	_float m_fTypeAccel = { 0.f };
	TYPE m_eType = { TYPE_END };

	_float m_fDirControl = { 1.f };

private:
	HRESULT Ready_Components();

public:
	static CTrail_Revolve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
