#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Logo() = default;

public:
	virtual HRESULT Initialize(_uint iLevelIndex) override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bLevelStart = { false };
	_bool m_bLevelOver = { false };

private:
	HRESULT Ready_Layer_BackGround(const _wstring& strLayerTag);
	HRESULT Ready_Layer_UI(const _wstring& strLayerTag);

public:
	static CLevel_Logo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevelIndex);
	virtual void Free() override;
};

END
