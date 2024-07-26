#pragma once

#include "VIBuffer.h"

/* 모델 = 메시 + 메시 + 메시 .. */
/* 메시로 구분하는 이유 : 파츠의 교체를 용이하게 만들어주기 위해서.*/
/* 메시 = 정점버퍼 + 인덱스 */

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;	

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint				m_iMaterialIndex = { 0 };

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END