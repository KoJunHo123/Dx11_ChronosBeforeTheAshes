#include "..\Public\Bone.h"

CBone::CBone()
{

}

HRESULT CBone::Initialize(ifstream* infile)
{	
	infile->read(reinterpret_cast<_char*>(m_szName), sizeof(_char) * MAX_PATH);
	infile->read(reinterpret_cast<_char*>(&m_TransformationMatrix), sizeof(_float4x4));
	infile->read(reinterpret_cast<_char*>(&m_iParentBoneIndex), sizeof(_int));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Update_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix PreTransformMatrix)
{
	if (-1 == m_iParentBoneIndex)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * PreTransformMatrix);
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * Bones[m_iParentBoneIndex]->Get_CombinedTransformationMatrix());
}

CBone* CBone::Create(ifstream* infile)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(infile)))
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
