#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Render(class CShader* pShader);

private:
	/* 내가 넣어준 경로에 해당하는 파일의 정보를 읽어서 aiScene객체에 저장해준다. */
	Assimp::Importer				m_Importer;

	/* 파일로부터 읽어온 모든 정보를 보관하고 있는다. */
	const aiScene*					m_pAIScene = { nullptr };
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	vector<class CMesh*>			m_Meshes;
	_float4x4						m_PreTransformMatrix = {};

private: 
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;


private:
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END