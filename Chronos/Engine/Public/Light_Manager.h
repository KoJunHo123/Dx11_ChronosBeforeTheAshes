#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	void Set_Pos(_uint iIndex, _fvector vPos);
	void Set_Diffuse(_uint iIndex, _fvector vColor);
	void Set_Range(_uint iIndex, _float fRange);

public:
	HRESULT Initialize();
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	list<class CLight*>				m_Lights;

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END