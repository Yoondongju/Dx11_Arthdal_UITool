#pragma once
#include "Base.h"
#include "Renderer.h"

BEGIN(Engine)
class CUI;

class CUI_Manager final 
	: public CBase
{
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Initialize();
	void Update();

	static void ResetFocusUI() { m_pFocusUI = nullptr; }


	void		SetFocusUI(CUI* _pUI);
	CUI*		GetFocusUI();


	CUI*		GetTargetUI(CUI* _pFocusUI);

public:
	class CGameInstance*		m_pGameInstance = { nullptr };

	static CUI*					m_pFocusUI;
	

public:
	static CUI_Manager* Create();
	virtual void Free() override;

};

END
