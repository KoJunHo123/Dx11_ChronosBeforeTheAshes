#include "../Public/KeyManager.h"

CKeyManager::CKeyManager()
{
}

HRESULT CKeyManager::Initialize()
{
    ::ZeroMemory(m_bKeyState, sizeof(m_bKeyState));

    return S_OK;
}

_bool CKeyManager::Key_Pressing(int _iKey)
{
    if (GetAsyncKeyState(_iKey) & 0x8000)
        return true;

    return false;
}

_bool CKeyManager::Key_Down(int _iKey)
{
    // 이전에 눌린 적이 없고, 현재 눌린 상태

    if ((!m_bKeyState[_iKey]) && (GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = !m_bKeyState[_iKey];
        return true;
    }
    else
    {
        m_bKeyState[_iKey] = GetAsyncKeyState(_iKey);
    }
    return false;
}

_bool CKeyManager::Key_Up(int _iKey)
{
    // 이전에 눌린 적이 있고, 현재는 눌리지 않은 상태

    if ((m_bKeyState[_iKey]) && !(GetAsyncKeyState(_iKey) & 0x8000))
    {
        m_bKeyState[_iKey] = !m_bKeyState[_iKey];
        return true;
    }

    // key 상태 복원
    for (int i = 0; i < VK_MAX; ++i)
    {
        if ((!m_bKeyState[i]) && (GetAsyncKeyState(_iKey) & 0x8000))
            m_bKeyState[i] = !m_bKeyState[i];
    }

    return false;
}

CKeyManager* CKeyManager::Create()
{
    CKeyManager* pInstance = new CKeyManager();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CKeyManager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CKeyManager::Free()
{
    __super::Free();
}
