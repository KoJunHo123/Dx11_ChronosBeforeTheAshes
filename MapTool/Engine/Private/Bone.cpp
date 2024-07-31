#include "..\Public\Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(const aiNode* pAIBone)
{
	strcpy_s(m_szName, pAIBone->mName.data);

	memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

CBone* CBone::Create(const aiNode* pAIBone)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAIBone)))
	{
		MSG_BOX(TEXT("Failed to Created : CBone"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CBone::Free()
{
	__super::Free();

}
