#pragma once

#include "Player_Part.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CPlayer_Shield final : public CPlayer_Part
{
public:
	typedef struct : public CPlayer_Part::PLAYER_PART_DESC
	{
		const _float4x4* pSocketBoneMatrix = { nullptr };
		class CTexture* pNoiseTextureCom = { nullptr };

		_float* pRatio = { nullptr };
	}PLAYER_SHIELD_DESC;
private:
	CPlayer_Shield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Shield(const CPlayer_Shield& Prototype);
	virtual ~CPlayer_Shield() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() override;

private:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CTexture* m_pNoiseTextureCom = { nullptr };

	const _float4x4* m_pSocketMatrix = { nullptr };
	_float* m_pRatio = { nullptr };

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Shield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};
END
