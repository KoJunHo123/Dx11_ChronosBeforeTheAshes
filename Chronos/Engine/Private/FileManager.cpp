#include "FileManager.h"

CFile_Manager::CFile_Manager()
{
}

HRESULT CFile_Manager::Initialize()
{
    m_SavePath = TEXT("../Bin/Resources/Models/");
    return S_OK;
}

void CFile_Manager::Add_SaveData(void* pArg, _uint iSize)
{
    SEPARATOR_DESC* pDesc = new SEPARATOR_DESC;
    pDesc -> pArg = pArg;
    pDesc->iSize = iSize;

    m_SeparatorDescs.emplace_back(pDesc);
}

SEPARATOR_DESC* CFile_Manager::Get_LoadedData(_uint iIndex)
{
    if (iIndex >= m_SeparatorDescs.size())
        return nullptr;

    return m_SeparatorDescs[iIndex];
}

void CFile_Manager::Clear()
{
    m_SavePath = TEXT("../Bin/Resources/Models/");
    for (auto& desc : m_SeparatorDescs)
        Safe_Delete(desc);
    m_SeparatorDescs.clear();
}

// 파일 이름, 확장자 따로 넣어주기.
HRESULT CFile_Manager::Save_File(_wstring strFileName, _wstring strExt)
{
    if (true == m_SeparatorDescs.empty())
        return E_FAIL;

    // Save 폴더까지. 없으면 만들기.
    if (!exists(m_SavePath))
    {
        if (!create_directory(m_SavePath))
            return E_FAIL;
    }

    // 파일 이름
    m_SavePath += strFileName;
    
    // Save 폴더 -> 파일이름으로 폴더 하나 만들기.
    if (!exists(m_SavePath))
    {
        if (!create_directory(m_SavePath))
            return E_FAIL;
    }

    // 파일 이름 + 확장자명 경로에다 붙이기.
    _wstring strFileNameExt = TEXT("/") + strFileName + TEXT(".") + strExt;
    m_SavePath += strFileNameExt;

    // app : 이어붙이기.
    ofstream outfile(m_SavePath, ios::binary | ios::app);

    _uint iCount = m_SeparatorDescs.size();
    if (outfile.is_open())
    {
        outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(iCount));
        for(auto& elem : m_SeparatorDescs)
        {
            outfile.write(reinterpret_cast<const char*>(&elem->iSize), sizeof(_uint));
            outfile.write(reinterpret_cast<const char*>(elem->pArg), elem->iSize);
        }
        outfile.close();
    }
    else
        return E_FAIL;

    Clear();


    return S_OK;
}

HRESULT CFile_Manager::Load_File(_wstring strFileName , _wstring strExt)
{
    // Save 폴더 까지.
    if (!exists(m_SavePath))
            return E_FAIL;

    // 파일 이름 + 확장자명 경로에다 붙이기.
    _wstring strFileNameExt = strFileName + TEXT("/") + strFileName + TEXT(".") + strExt;
    m_SavePath += strFileNameExt;

    ifstream infile(m_SavePath, ios::binary);

    if (infile.is_open())
    {
        infile.seekg(0, ios::beg);
        infile.read(reinterpret_cast<char*>(&m_iSaveCount), sizeof(m_iSaveCount));
        
        for(_uint i = 0; i < m_iSaveCount; ++i)
        {
            SEPARATOR_DESC* pDesc = new SEPARATOR_DESC;
            
            infile.read(reinterpret_cast<char*>(&pDesc->iSize), sizeof(pDesc->iSize));
            infile.read(reinterpret_cast<char*>(pDesc->pArg), pDesc->iSize);

            m_SeparatorDescs.emplace_back(pDesc);
        }
        
        infile.close();
    }
    else
        return E_FAIL;

    return S_OK;
}

CFile_Manager* CFile_Manager::Create()
{
    CFile_Manager* pInstance = new CFile_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CFile_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFile_Manager::Free()
{
    __super::Free();
    Clear();
}
