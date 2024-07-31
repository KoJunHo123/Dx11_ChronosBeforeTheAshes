#pragma once
#include "Base.h"
#include <filesystem>
#include <fstream>

using namespace filesystem;

BEGIN(Engine)
class CFile_Manager final : public CBase
{
public:


private:
	CFile_Manager();
	virtual ~CFile_Manager() = default;

public:
	size_t Get_LoadedDataCount() {
		return m_SeparatorDescs.size();
	}

	void Set_SaveFilePath(_wstring strFilePath) {
		m_SavePath = strFilePath;
	}

public:
	HRESULT Initialize();
	void Add_SaveData(void* pArg, _uint iSize);
	SEPARATOR_DESC* Get_LoadedData(_uint iIndex);
	void Clear();
	 

	// 확장자까지 받아야 함.
	HRESULT Save_File(_wstring strFileName, _wstring strExt);
	HRESULT Load_File(_wstring strFileName, _wstring strExt);

private:
	path m_SavePath = TEXT("");
	vector<SEPARATOR_DESC*> m_SeparatorDescs;
	_uint					m_iSaveCount = { 0 };

public:
	static CFile_Manager* Create();
	virtual void Free();
};
END
