#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAIBone, _int iParentBoneIndex)
{
    m_iParentBoneIndex = iParentBoneIndex;
    // 뼈 이름 복사
    strcpy_s(m_szName, pAIBone->mName.data);
    // 자료형이 달라도 형태가 똑같으니까 메모리 그대로 복사
    memcpy(&m_TransformationMatrix, &pAIBone->mTransformation, sizeof(_float4x4));
    // 이거 Assimp에서 가져오는 행렬이 RUL이 열 기준으로 되어 있어서 전치를 통해 행기준으로 바꿔줌
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    // CombinedTransformation -> 이거 부모 행렬 받아서 만드는 최종행렬인데 매 프레임마다 바꿔줘야 되는 거라서 일단 항등으로 초기화.
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

void CBone::Save_Data(ofstream* outfile)
{
    outfile->write(reinterpret_cast<const char*>(m_szName), sizeof(_char) * MAX_PATH);
    outfile->write(reinterpret_cast<const char*>(&m_TransformationMatrix), sizeof(_float4x4));
    outfile->write(reinterpret_cast<const char*>(&m_iParentBoneIndex), sizeof(_int));
}

CBone* CBone::Create(const aiNode* pAIBone, _int iParentBoneIndex)
{
    CBone* pInstance = new CBone;

    if (FAILED(pInstance->Initialize(pAIBone, iParentBoneIndex)))
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
