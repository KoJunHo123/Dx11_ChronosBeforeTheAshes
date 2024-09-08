#pragma once

#include "PartObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CTransform;
class CNavigation;
class CModel;
class CShader;
END

BEGIN(Client)
class CLab_Mage_Body final : public CPartObject
{
public:
	typedef struct : public CPartObject::PARTOBJ_DESC
	{
		class CTransform* pMage_TransformCom = { nullptr };
		class CNavigation* pNavigationCom = { nullptr };

		_uint* pState = { nullptr };
		_bool* pIsFinished = { nullptr };
		_int* pHP = { nullptr };
		_float* pDistance = { nullptr };
		_bool* pAnimStart = { false };
		_bool* pAnimOver = { false };
		_bool* pAttackActive = { nullptr };
	}BODY_DESC;

private:
	CLab_Mage_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLab_Mage_Body(const CLab_Mage_Body& Prototype);
	virtual ~CLab_Mage_Body() = default;

public:
	void Set_HittedDesc(_float fHittedAngle, _int iImpactedDamage) {
		m_fHittedAngle = fHittedAngle;
		m_iImpactedDamage = iImpactedDamage;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Reset_Animation();
	_uint Get_FrameIndex();
	const _float4x4* Get_BoneMatrix_Ptr(const _char* pBoneName) const;


private:
	_bool Animation_Loop();
	_bool Animation_NonInterpolate();

	_vector Find_TeleportPos();

private:
	class CTransform* m_pMage_TransformCom = { nullptr };
	class CTransform* m_pPlayer_TransformCom = { nullptr };

	class CNavigation* m_pNavigationCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };

private:
	MAGE_ANIM m_eMageAnim = { MAGE_ANIM_END };
	_float m_fHittedAngle = { 0.f };
	_int m_iImpactedDamage = { 0 };

	_float m_fSpeed = { 0.f };

private:
	_uint* m_pState = { nullptr };
	_bool* m_pIsFinished = { nullptr };
	_int* m_pHP = { nullptr };
	_float* m_pDistance = { nullptr };
	_bool* m_pAnimStart = { false };
	_bool* m_pAnimOver = { false };
	_bool* m_pAttackActive = { nullptr };

private:
	HRESULT Ready_Components();

private:
	void Play_Animation(_float fTimeDelta);

public:
	static CLab_Mage_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END
