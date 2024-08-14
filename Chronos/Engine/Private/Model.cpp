#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CModel::CModel(const CModel& Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes{ Prototype.m_iNumMeshes }
	, m_Meshes{ Prototype.m_Meshes }
	, m_PreTransformMatrix( Prototype.m_PreTransformMatrix )
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials{ Prototype.m_Materials }
	, m_iCurrentAnimIndex{ Prototype.m_iCurrentAnimIndex }
	, m_iNextAnimIndex { Prototype.m_iNextAnimIndex }
	, m_iNumAnimations {Prototype.m_iNumAnimations}
{
	for (auto& pPrototypeAnimation : Prototype.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());


	for (auto& pPrototypeBone : Prototype.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);


}

_uint CModel::Get_BoneIndex(const _char* pBoneName) const
{
	_uint iBoneIndex = { 0 };
	// 이름 비교해서 참 나올때까지 반복문
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			if (0 == strcmp(pBone->Get_Name(), pBoneName))
				return true;
			++iBoneIndex;
			return false;
		});

	if (iter == m_Bones.end())
		MSG_BOX(TEXT("없는데?"));

	return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	strcpy_s(m_szModelFilePath, pModelFilePath);

	// 뼈는 무조건 메쉬보다 먼저.
	if (FAILED(Ready_Bones()))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials()))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

_bool CModel::Play_Animation(_float fTimeDelta, _vector& vRootBoneChanged)
{
	// 해당하는 애니메이션 인덱스를 업데이트
	_bool isFinished = { false };
	_bool isChanged = { false };

	if (m_iNextAnimIndex != m_iCurrentAnimIndex)
	{
		isChanged = m_Animations[m_iCurrentAnimIndex]->Update_ChangeAnimation(m_Animations[m_iNextAnimIndex], m_Bones, fTimeDelta);

		if (true == isChanged)
		m_iCurrentAnimIndex = m_iNextAnimIndex;
	}
	else
		 isFinished = m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrices(m_Bones, m_isLoop, fTimeDelta);

	if (true == isFinished)
		XMStoreFloat4(&m_vTranslationChange, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	// 이동량 구해주고
	vRootBoneChanged = m_Bones[1]->Get_TransformationMatrix().r[3] - XMLoadFloat4(&m_vTranslationChange);
	// 이전 채널을 현재 채널로 하고
	XMStoreFloat4(&m_vTranslationChange, m_Bones[1]->Get_TransformationMatrix().r[3]);
	// 현재 루트 본을 항등으로.
	m_Bones[1]->Set_TransformationMatrix(XMMatrixIdentity());

	for (auto& Bone : m_Bones)
	{
		Bone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}

	return isFinished;
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);
}

HRESULT CModel::Bine_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	return 	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);
}

HRESULT CModel::Ready_Meshes()
{
	_char MeshFilePath[MAX_PATH]{};
	strcpy_s(MeshFilePath, m_szModelFilePath);
	strcat_s(MeshFilePath, ".mesh");

	ifstream infile(MeshFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	infile.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, this, &infile, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	infile.close();
	return S_OK;
}

HRESULT CModel::Ready_Materials()
{
	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szModelFilePath);
	strcat_s(MaterialFilePath, ".material");

	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	infile.read(reinterpret_cast<char*>(&m_iNumMaterials), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};

		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			// pAIMaterial->GetTextureCount(j);
			_char				strTexturePath[MAX_PATH];

			_char				szDrive[MAX_PATH] = "";
			_char				szDirectory[MAX_PATH] = "";
			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";

			_char				szTexturePath[MAX_PATH] = "";


			infile.read(reinterpret_cast<_char*>(strTexturePath), sizeof(_char) * MAX_PATH);

			if (0 == strTexturePath[0])
				continue;

			_splitpath_s(m_szModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


			strcpy_s(szTexturePath, szDrive);
			strcat_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szFileName);
			strcat_s(szTexturePath, szExt);

			_tchar				szFinalPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath, strlen(szTexturePath), szFinalPath, MAX_PATH);
			
			MeshMaterial.pMaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szFinalPath, 1);
			if (nullptr == MeshMaterial.pMaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}
	infile.close();

	return S_OK;
}

HRESULT CModel::Ready_Bones()
{
	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szModelFilePath);
	strcat_s(MaterialFilePath, ".bone");

	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	// 일단 뼈 만듬.
	_uint iBoneSize = 0;
	infile.read(reinterpret_cast<_char*>(&iBoneSize), sizeof(_uint));
	for (size_t i = 0; i < iBoneSize; ++i)
	{
		CBone* pBone = CBone::Create(&infile);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.emplace_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szModelFilePath);
	strcat_s(MaterialFilePath, ".animation");
	ifstream infile(MaterialFilePath, ios::binary);

	if (!infile.is_open())
		return E_FAIL;

	infile.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof(_uint));
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(&infile, this, m_Bones.size());
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	infile.close();

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType, const _char* pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_Release(pTexture);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
	{
		Safe_Release(pAnimation);
	}
	m_Animations.clear();
}
