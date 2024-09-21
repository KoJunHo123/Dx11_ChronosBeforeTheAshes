#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instancing
{
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Spread(_fvector vPivot, _fvector vLocalLook, _float fSpeed, _float fTimeDelta);
	virtual _bool Move_Dir(_fvector vDir, _fvector vLocalLook, _float fSpeed, _float fTimeDelta);
	virtual _bool Converge(_fvector vPivot, _fvector vLocalLook, _float fSpeed, _float fTimeDelta);

	void Scaling(_fvector vAddScale, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fRotatioPerSec, _float fTimeDelta);

private:
	_float Get_Radian(_fvector vMoveDir);

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};


END