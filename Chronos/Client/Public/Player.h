#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CFSM;
class CState;
END

BEGIN(Client)
class CPlayer final : public CGameObject
{
public:
	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_vector vPos;
		_float4 vScale;
		_vector vRotationAxis;
		_float fRotationAngle;
	}PLAYER_DESC;

	enum PLAYER_STATE{ STATE_MOVE, STATE_ATTACK, STATE_JUMP, STATE_BLOCK, STATE_IMPACT, STATE_ACTION, STATE_END};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT Load_Data(ifstream* pInFile) override;

public:
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	class CFSM* m_pFSM = { nullptr };

	_float m_fSpeed = { 0.f };
	
private:
	HRESULT Ready_Components();
	HRESULT Ready_States();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};
END
