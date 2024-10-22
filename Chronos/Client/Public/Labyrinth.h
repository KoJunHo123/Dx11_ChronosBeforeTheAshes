#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CLabyrinth final : public CGameObject
{
public:
	enum COLOR { COLOR_YELLOW, COLOR_PUPPLE, COLOR_RED, COLOR_DEFAULT, COLOR_END };

private:
	CLabyrinth(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLabyrinth(const CLabyrinth& Prototype);
	virtual ~CLabyrinth() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_LightDepth();

private:
	class CShader*				m_pShaderCom = { nullptr };	
	class CModel*				m_pModelCom = { nullptr };

	_float4 m_vColors[COLOR_END] = {};

private:
	HRESULT Ready_Components();

	COLOR Select_Color(_uint iMeshIndex);

public:
	static CLabyrinth* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual CGameObject* Pooling() override;
	virtual void Free() override;
};

END