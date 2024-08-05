#pragma once

#include "Component.h"
#include "Bone.h"



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

	// 메쉬에 모델 타입이 아니라 모델을 전달할 것이기 때문에 타입을 가져오는 함수 만든거.
	TYPE Get_ModelType() const {
		return m_eType;
	}

	// 메쉬가 뼈의 인덱스를 벡터 컨테이너로 저장하는데 거기다 전달해줄 용.
	// 모델 기준 뼈의 인덱스를 가져옴.
	// 모델의 뼈와 메쉬의 뼈는 인덱스가 다르지만 이름이 똑같음. 그러니까 이름으로 구별해주는 것.
	_uint Get_BoneIndex(const _char* pBoneName) const;

	// 뼈에서 매 프레임 업데이트 되는 컴파인(최종)행렬 받아오려고.
	_matrix Get_BoneCombindTransformationMatrix(_uint iBoneIndex) const {
		return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
	}


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;	
	virtual HRESULT Render(_uint iMeshIndex);

public:
	void Play_Animation(_float fTimeDelta);

public:
	HRESULT Bind_Material(class CShader * pShader, const _char * pConstantName, aiTextureType eMaterialType, _uint iMeshIndex);	
	// 머테리얼 묶을 떄 처럼 몇번째 메쉬의 뼈를 묶을거냐 이런 느낌.
	HRESULT Bine_MeshBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

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
	vector<class CBone*>			m_Bones;

private:
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;

private:
	_char m_szFilePath[MAX_PATH] = {};

public:
	HRESULT	Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAIBone, _int iParentBoneIndex);
	HRESULT Ready_Animation();

	HRESULT Ready_Bones_With_Save(const aiNode* pAIBone, _int iParentBoneIndex);

	HRESULT Create_Directory(_char szFileName[MAX_PATH]);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END