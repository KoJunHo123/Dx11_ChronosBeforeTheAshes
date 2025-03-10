#pragma once

#include "Component.h"
#include "Bone.h"

using namespace filesystem;

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
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	TYPE Get_ModelType() const {
		return m_eType;
	}
	_uint Get_BoneIndex(const _char* pBoneName) const;

	_matrix Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const {
		return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
	}

public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Create_Cells(class CNavigation* pNavigation, _fvector vTerrainPos);

public:
	void SetUp_Animation(_uint iAnimationIndex, _bool isLoop = false) {
		m_iNextAnimIndex = iAnimationIndex;
		m_isLoop = isLoop;
		XMStoreFloat4(&m_vTranslationChange, m_Bones[1]->Get_TransformationMatrix().r[3]);
	}

	_bool Play_Animation(_float fTimeDelta, _vector& vRootBoneChanged);


public:
	HRESULT Bind_Material(class CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex);
	HRESULT Bine_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	_bool isPicking(const _matrix& WorldMatrix, _vector* pOut, _uint* pMeshIndex);
	void Render_OnOff() {
		m_isRender = !m_isRender;
	}

private:
	TYPE							m_eType = { TYPE_END };

private: /* 메시의 정보를 저장한다. */
	_uint							m_iNumMeshes = { 0 };
	_float4x4						m_PreTransformMatrix = {};
	vector<class CMesh*>			m_Meshes;

private:
	_uint							m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>			m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_bool							m_isLoop = { false };
	
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNextAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

private:
	_char m_szModelFilePath[MAX_PATH] = {};
	_float4 m_vTranslationChange = {};

	_bool m_isRender = { true };

private:
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials();
	HRESULT Ready_Bones();
	HRESULT Ready_Animations();

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END