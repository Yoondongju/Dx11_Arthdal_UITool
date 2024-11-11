#pragma once
#include "Base.h"

BEGIN(Engine)

class CKey_Manager final : public CBase
{
public:
	typedef struct
	{
		KEY_STATE	eState;  // 키의 상태값
		bool		bPrev;   // 이전 프레임에서 눌렸는지 안눌렸는지 
	}KEYINFO;

private:
	CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	KEY_STATE	Get_KeyState(KEY _eKey) { return m_vecKey[(int)_eKey].eState; }
	_float2		Get_MousePos() { return m_vMousePos; }


public:
	HRESULT Initialize(HWND hWnd);
	void Update();

private:
	HWND				m_hWnd = {};
	_float2				m_vMousePos = {};
	vector<KEYINFO>		m_vecKey;

public:
	static CKey_Manager* Create(HWND hWnd);
	virtual void Free() override;

};

END