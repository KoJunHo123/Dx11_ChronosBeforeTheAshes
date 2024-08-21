#pragma once

#include "Player_Part.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CPlayer_Body final : public CPlayer_Part
{
public:
	typedef struct : public CPlayer_Part::PLAYER_PART_DESC
	{
		class CTransform* pPlayerTransformCom = { nullptr };
	}PLAYER_BODY_DESC;
private:
	CPlayer_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Body(const CPlayer_Body& Prototype);
	virtual ~CPlayer_Body() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_uint Get_FrameIndex();

private:
	class CTransform* m_pPlayerTransformCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	void Play_Animation(_float fTimeDelta);	
	_vector Get_Rotation(_matrix WorldMatrix, _vector vExist);

private:
	HRESULT Ready_Components();

public:
	static CPlayer_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
END
