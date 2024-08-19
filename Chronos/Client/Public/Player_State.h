#pragma once

#include "State.h"
#include "Client_Defines.h"
#include "Transform.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTransform;
END

BEGIN(Client)
class CPlayer_State abstract : public CState
{
public:
	typedef struct : public CState::STATE_DESC
	{
		class CTransform* pTransformCom = { nullptr };
		class CShader* pShaderCom = { nullptr };
		class CModel* pModelCom = { nullptr };
		_float* pSpeed = { nullptr };
	}PLAYER_STATE_DESC;

protected:
	CPlayer_State();
	virtual ~CPlayer_State() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT StartState(void** pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT ExitState(void** pArg) override;

protected:
	_vector Get_Rotation(_matrix WorldMatrix, _vector vExist);
	void Play_Animation(_float fTimeDelta);

	void Look_CameraDir();

protected:
	class CTransform* m_pTransformCom = { nullptr };
	class CShader* m_pShaderCom = { nullptr };
	class CModel* m_pModelCom = { nullptr };
	_float* m_pSpeed = { nullptr };

	ANIM_PLAYER m_ePlayerAnim = { PLAYER_ANIM_END };

	// 애니메이션 종료시
	_bool m_isFinished = { false };
	// 애니메이션 돌아가는 동안 입력 안받는 용도
	_bool m_bMotionLock = { false };

	// 애니메이션이 변경될 떄 어느 정도 변경되었는지.
	_float m_fChangeRate = { 0.f };

	// 애니메이션 테스트
	_float4 m_vTest = {};

public:
	virtual void Free();
};
END