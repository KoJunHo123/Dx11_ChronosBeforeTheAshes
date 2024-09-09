#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)			{ 
		return m_byKeyState[byKeyID]; }

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	{ 	
		return m_tMouseState.rgbButtons[eMouse]; 	
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{			
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}

	_bool Get_DIKeyState_Down(_ubyte byKeyID);
	_bool Get_DIKeyState_Up(_ubyte byKeyID);
	_bool Get_DIMouseState_Down(MOUSEKEYSTATE eMouse);
	_bool Get_DIMouseState_Up(MOUSEKEYSTATE eMouse);
	
	
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };

private:	
	_byte					m_byKeyState[256] = {};			// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byPreKeyState[256] = {};

	DIMOUSESTATE			m_tMouseState = {};
	DIMOUSESTATE			m_tPreMouseState = {};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END

