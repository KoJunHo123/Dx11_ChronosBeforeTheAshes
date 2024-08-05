#include "..\Public\Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{

}

CModel::CModel(const CModel & Prototype)
	: CComponent{ Prototype }
	, m_iNumMeshes { Prototype.m_iNumMeshes } 
	, m_Meshes{ Prototype.m_Meshes }
	, m_iNumMaterials{ Prototype.m_iNumMaterials }
	, m_Materials { Prototype.m_Materials}
	, m_Bones{ Prototype.m_Bones }
	, m_PreTransformMatrix{ Prototype.m_PreTransformMatrix }
	, m_Animations{Prototype.m_Animations}
	, m_iNumAnimations{Prototype.m_iNumAnimations}
{
	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterialTextures)
			Safe_AddRef(pTexture);
	}

	for (auto& pMesh : m_Meshes)	
		Safe_AddRef(pMesh);

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

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

	return iBoneIndex;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	_uint		iFlag = { 0 };	
	
	/* 이전 : 모든 메시가 다 원점을 기준으로 그렺니다. */
	/* 이후 : 모델을 구성하는 모든 메시들을 각각 정해진 상태(메시를 배치하기위한 뼈대의 위치에 맞춰서)대로 미리 변환해준다.*/

	iFlag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;	
	
	if (eType == TYPE_NONANIM)
		iFlag |= aiProcess_PreTransformVertices;	

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	_char szFileName[MAX_PATH]{};
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	if (FAILED(Create_Directory(szFileName)))
		return E_FAIL;

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);
	m_eType = eType;

	if (FAILED(Ready_Bones_With_Save(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;	

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();	

	return S_OK;
}

void CModel::Play_Animation(_float fTimeDelta)
{
	// 뼈 움직이고

	// 움직인 뼈를 갱신.
	for (auto& Bone : m_Bones)
	{
		Bone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
	}
}

HRESULT CModel::Bind_Material(CShader* pShader, const _char* pConstantName, aiTextureType eMaterialType, _uint iMeshIndex)
{
	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	return m_Materials[iMaterialIndex].pMaterialTextures[eMaterialType]->Bind_ShadeResource(pShader, pConstantName, 0);	
}

HRESULT CModel::Bine_MeshBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_BoneMatrices(this, pShader, pConstantName);

	return S_OK;
}

HRESULT CModel::Ready_Meshes()
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	_char MeshFilePath[MAX_PATH]{};
	strcpy_s(MeshFilePath, m_szFilePath);
	strcat_s(MeshFilePath, ".mesh");

	ofstream outfile(MeshFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;

	outfile.write(reinterpret_cast<const char*>(&m_iNumMeshes), sizeof(_uint));

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, this, m_pAIScene->mMeshes[i], &outfile, XMLoadFloat4x4(&m_PreTransformMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	outfile.close();
	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char * pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szFilePath);
	strcat_s(MaterialFilePath, ".material");

	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;

	outfile.write(reinterpret_cast<const char*>(&m_iNumMaterials), sizeof(_uint));
	
	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		MESH_MATERIAL		MeshMaterial{};
		
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];
		
		for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
		{			
			// pAIMaterial->GetTextureCount(j);
			aiString			strTexturePath;

			_char				szDrive[MAX_PATH] = "";
			_char				szDirectory[MAX_PATH] = "";
			_char				szFileName[MAX_PATH] = "";
			_char				szExt[MAX_PATH] = "";
			 
			_char				szTexturePath[MAX_PATH] = "";


			_char				szSavePath[MAX_PATH] = "";

			HRESULT hr = pAIMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath);
			strcpy_s(szSavePath, strTexturePath.data);
			outfile.write(reinterpret_cast<const char*>(szSavePath), sizeof(_char) * MAX_PATH);

			if (FAILED(hr))
				continue;

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);


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
	outfile.close();

	return S_OK;
}

HRESULT CModel::Ready_Bones_With_Save(const aiNode* pAIBone, _int iParentBoneIndex)
{
	/*  뼈대의 정보를 로드한다. 뼈대 ( aiNode, aiBone, aiAnimNode ) */
	if (FAILED(Ready_Bones(pAIBone, iParentBoneIndex)))
		return E_FAIL;

	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szFilePath);
	strcat_s(MaterialFilePath, ".bone");
	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;

	_uint iSize = (_uint)m_Bones.size();
	outfile.write(reinterpret_cast<const _char*>(&iSize), sizeof(_uint));
	
	for (size_t i = 0; i < m_Bones.size(); ++i)
	{
		m_Bones[i]->Save_Data(&outfile);
	}

	outfile.close();

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAIBone, _int iParentBoneIndex)
{
	// 일단 뼈 만듬.
	CBone* pBone = CBone::Create(pAIBone, iParentBoneIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_int		iParentIndex = m_Bones.size() - 1;
	// 해당뼈의 자식들도 만듬.
	// -> 자식 -> 자식의 자식 -> 자식의 자식의 자식... 이런 순으로 재귀로 싹 다 만들어줌.
	for (size_t i = 0; i < pAIBone->mNumChildren; ++i)
	{
		Ready_Bones(pAIBone->mChildren[i], iParentIndex);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	_char MaterialFilePath[MAX_PATH]{};
	strcpy_s(MaterialFilePath, m_szFilePath);
	strcat_s(MaterialFilePath, ".animation");
	ofstream outfile(MaterialFilePath, ios::binary);

	if (!outfile.is_open())
		return E_FAIL;

	m_iNumAnimations = m_pAIScene->mNumAnimations;

	outfile.write(reinterpret_cast<const _char*>(&m_iNumAnimations), sizeof(_uint));
	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], &outfile);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	outfile.close();

	return S_OK;
}


HRESULT CModel::Create_Directory(_char szFileName[MAX_PATH])
{
	strcpy_s(m_szFilePath, "../Bin/Resources/SaveModels/");

	if (!filesystem::exists(m_szFilePath))
	{
		if (!filesystem::create_directory(m_szFilePath))
			return E_FAIL;
	}

	strcat_s(m_szFilePath, szFileName);
	strcat_s(m_szFilePath, "/");

	// Save 폴더 -> 파일이름으로 폴더 하나 만들기.
	if (!filesystem::exists(m_szFilePath))
	{
		if (!filesystem::create_directory(m_szFilePath))
			return E_FAIL;
	}

	strcat_s(m_szFilePath, "/");
	strcat_s(m_szFilePath, szFileName);

	return S_OK;
}


CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CModel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

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
