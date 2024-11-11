#include "../Public/UI_Manager.h"
#include "../Public/GameInstance.h"


#include "UIBatch.h"
#include "../Public/UI.h"

CUI* CUI_Manager::m_pFocusUI = nullptr;


CUI_Manager::CUI_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUI_Manager::Initialize()
{
	return S_OK;
}

void CUI_Manager::Update()
{
	list<CGameObject*>& UILists = m_pGameInstance->Get_RenderList(CRenderer::RG_UI);
	UILists.sort([](CGameObject* a, CGameObject* b) -> bool								// 소팅 조진다
		{
			CUI* pDest = static_cast<CUI*>(a);
			CUI* pSrc = static_cast<CUI*>(b);
	
			return pDest->Get_CurZOrder() > pSrc->Get_CurZOrder();
		});


	m_pFocusUI = GetFocusUI();	// 포커싱할 렌더그룹을찾는다.

	if (m_pFocusUI)
	{
		//Dead 처리를 어떻게하지
		/*if (m_pFocusUI->Get_Dead())
		{
			m_pFocusUI = nullptr;
			return;
		}			*/
	}
	else
		return;


	UILists.sort([](CGameObject* a, CGameObject* b) -> bool								// 소팅 조진다
		{
			CUI* pDest = static_cast<CUI*>(a);
			CUI* pSrc = static_cast<CUI*>(b);

			return pDest->Get_CurZOrder() > pSrc->Get_CurZOrder();
		});



	CUI* pTargetUI = nullptr;

	if (CUI::UI_TYPE::BATCHING != m_pFocusUI->Get_UIType())
		pTargetUI = GetTargetUI(m_pFocusUI);	// 배칭UI가 아닐경우에만 자식이잇으니까
	else
		pTargetUI = m_pFocusUI;
	

	bool bLbtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;   // 딱 뗀 시점

	if (pTargetUI)
	{	
		if (bLbtnTap)
		{
			pTargetUI->MouseLBtnDown();
			pTargetUI->m_bLbtnDown = true;
		}
		else if (bLbtnAway)
		{
			pTargetUI->MouseLBtnUp();
			
			if (pTargetUI->m_bLbtnDown)
				pTargetUI->MouseClick();

			pTargetUI->m_bLbtnDown = false;
		}
	}



	// 로직순서가  UI매니저 업뎃돌고 -> 렌더러에서 그리는 순서지
	// 유아이매니저에서 포커스된애들 체크할때 소팅이 된 상태에서 포커스 체크가 들어가야하는건 맞아  (이때 소팅을하고)
	// 타겟으로 정해졋으면 또 소팅을해 ?

}


void CUI_Manager::SetFocusUI(CUI* _pUI)
{
	m_pFocusUI = _pUI;
}



CUI* CUI_Manager::GetFocusUI()
{
	// 이따 논하이라키,디폴트,하이라키 UI 렌더그룹 세개를 하나로 합친거로 순회할거임
	list<CGameObject*>& UILists = m_pGameInstance->Get_RenderList(CRenderer::RG_UI);		// 렌더러에 추가한 다음의 시점에서 가져온다음 

	{
		bool bLbtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
		bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;

		bool bLbtnHold = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::HOLD;

		if (0 == UILists.size())
		{
			m_pFocusUI = nullptr;
		}

		CUI* pFocusUI = m_pFocusUI;
		CUI* pUI = nullptr;

		list<CGameObject*>::iterator targetiter = UILists.end();
		list<CGameObject*>::iterator iter = UILists.begin();

		if (!bLbtnTap)				// 내가 누른적이 없다면 포커싱은 안주되, MouseOn은 호출해줘야함..
		{
			return pFocusUI;		// 이전에 포커싱햇던거 그대로 리턴		(마우스On/Out은 CUI의 MouseOnCheck에서할거)
		}

		if (bLbtnTap || bLbtnHold)
		{
			for (iter; iter != UILists.end(); ++iter)
			{
				pUI = ((CUI*)(*iter));

				if (CUI::UI_TYPE::BATCHING == pUI->Get_UIType())
				{
					list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = static_cast<CUIBatch*>(pUI)->Get_UIList();
					_bool	isSelfFocus = false;	// 자기 자신이 포커싱된경우 continue
					for (auto UIInfo : UIList)
					{
						if (pUI->IsActivate() &&  UIInfo.pUI->IsMouseOn())
						{
							pFocusUI = UIInfo.pUI;		// 배칭UI에 속한 짤짤이 UI고 얘네는 소팅이 X
							pUI->Set_CurZOrder(0.f);	// 실제 그 배칭UI의 Z를 0으로 ㄱ
							isSelfFocus = true;
							break;
						}
					}	
					if(true == isSelfFocus)
						continue;
				}
				else
				{
					if (pUI->IsActivate() && pUI->IsMouseOn())
					{
						targetiter = iter;
					}					
				}	
				_float fCurZ = pUI->Get_CurZOrder();
				pUI->Set_CurZOrder(++fCurZ);
			}
		}

		if (targetiter == UILists.end())    // UI를 픽하지않았거나 Layer_UIBatch의 UI를 클릭햇으면 targetiter값이 안채워져잇음
		{
			return pFocusUI;				// 이전에 포커싱햇던거 그대로 리턴
		}

		pFocusUI = (CUI*)(*targetiter);	    // 이번프레임에 포커싱 될 UI 새로 갱신
		pFocusUI->Set_CurZOrder(0.f);

		return pFocusUI;
	}
}

CUI* CUI_Manager::GetTargetUI(CUI* _pFocusUI)
{
	if (nullptr == _pFocusUI)
		return nullptr;

	CUI* pTargetUI = nullptr;

	static list<CUI*> queue;
	static vector<CUI*> vecNoneTarget;
	
	queue.push_back(_pFocusUI);
	while (!queue.empty())			// 안에 들어있는 자식들이 있을때까지 돌고
	{
		CUI* pUI = queue.front();
		queue.pop_front();

		if (CUI::UI_TYPE::BATCHING == pUI->Get_UIType())   
		{
			list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = static_cast<CUIBatch*>(pUI)->Get_UIList();
			for (auto UIInfo : UIList)    
			{
				if (UIInfo.pUI->IsMouseOn())
				{		
					pTargetUI = UIInfo.pUI;		  // 배칭 UI가 자식으로 들어온경우 파생되는 자식은 더이상없어
					return pTargetUI;
				}
			}
		}

		if (pUI->IsMouseOn())
		{
			if (nullptr != pTargetUI)
			{
				vecNoneTarget.push_back(pTargetUI);
			}
			pTargetUI = pUI;
		}
		else
		{
			vecNoneTarget.push_back(pUI);
		}

		const list<CUI*>& PlayerUIList = pUI->GetChildUI();

		for (auto& iter : PlayerUIList)
		{
			queue.push_back(iter);
		}
	}

	vecNoneTarget.clear();

	return pTargetUI;
}

CUI_Manager* CUI_Manager::Create()
{
	CUI_Manager* pInstance = new CUI_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pFocusUI);
}
