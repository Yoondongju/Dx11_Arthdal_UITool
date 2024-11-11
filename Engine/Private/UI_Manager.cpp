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
	UILists.sort([](CGameObject* a, CGameObject* b) -> bool								// ���� ������
		{
			CUI* pDest = static_cast<CUI*>(a);
			CUI* pSrc = static_cast<CUI*>(b);
	
			return pDest->Get_CurZOrder() > pSrc->Get_CurZOrder();
		});


	m_pFocusUI = GetFocusUI();	// ��Ŀ���� �����׷���ã�´�.

	if (m_pFocusUI)
	{
		//Dead ó���� �������
		/*if (m_pFocusUI->Get_Dead())
		{
			m_pFocusUI = nullptr;
			return;
		}			*/
	}
	else
		return;


	UILists.sort([](CGameObject* a, CGameObject* b) -> bool								// ���� ������
		{
			CUI* pDest = static_cast<CUI*>(a);
			CUI* pSrc = static_cast<CUI*>(b);

			return pDest->Get_CurZOrder() > pSrc->Get_CurZOrder();
		});



	CUI* pTargetUI = nullptr;

	if (CUI::UI_TYPE::BATCHING != m_pFocusUI->Get_UIType())
		pTargetUI = GetTargetUI(m_pFocusUI);	// ��ĪUI�� �ƴҰ�쿡�� �ڽ��������ϱ�
	else
		pTargetUI = m_pFocusUI;
	

	bool bLbtnTap = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::TAP;
	bool bLbtnAway = m_pGameInstance->Get_KeyState(KEY::LBUTTON) == KEY_STATE::AWAY;   // �� �� ����

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



	// ����������  UI�Ŵ��� �������� -> ���������� �׸��� ������
	// �����̸Ŵ������� ��Ŀ���Ⱦֵ� üũ�Ҷ� ������ �� ���¿��� ��Ŀ�� üũ�� �����ϴ°� �¾�  (�̶� �������ϰ�)
	// Ÿ������ ���ؠ����� �� �������� ?

}


void CUI_Manager::SetFocusUI(CUI* _pUI)
{
	m_pFocusUI = _pUI;
}



CUI* CUI_Manager::GetFocusUI()
{
	// �̵� �����̶�Ű,����Ʈ,���̶�Ű UI �����׷� ������ �ϳ��� ��ģ�ŷ� ��ȸ�Ұ���
	list<CGameObject*>& UILists = m_pGameInstance->Get_RenderList(CRenderer::RG_UI);		// �������� �߰��� ������ �������� �����´��� 

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

		if (!bLbtnTap)				// ���� �������� ���ٸ� ��Ŀ���� ���ֵ�, MouseOn�� ȣ���������..
		{
			return pFocusUI;		// ������ ��Ŀ���޴��� �״�� ����		(���콺On/Out�� CUI�� MouseOnCheck�����Ұ�)
		}

		if (bLbtnTap || bLbtnHold)
		{
			for (iter; iter != UILists.end(); ++iter)
			{
				pUI = ((CUI*)(*iter));

				if (CUI::UI_TYPE::BATCHING == pUI->Get_UIType())
				{
					list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = static_cast<CUIBatch*>(pUI)->Get_UIList();
					_bool	isSelfFocus = false;	// �ڱ� �ڽ��� ��Ŀ�̵Ȱ�� continue
					for (auto UIInfo : UIList)
					{
						if (pUI->IsActivate() &&  UIInfo.pUI->IsMouseOn())
						{
							pFocusUI = UIInfo.pUI;		// ��ĪUI�� ���� ©©�� UI�� ��״� ������ X
							pUI->Set_CurZOrder(0.f);	// ���� �� ��ĪUI�� Z�� 0���� ��
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

		if (targetiter == UILists.end())    // UI�� �������ʾҰų� Layer_UIBatch�� UI�� Ŭ�������� targetiter���� ��ä��������
		{
			return pFocusUI;				// ������ ��Ŀ���޴��� �״�� ����
		}

		pFocusUI = (CUI*)(*targetiter);	    // �̹������ӿ� ��Ŀ�� �� UI ���� ����
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
	while (!queue.empty())			// �ȿ� ����ִ� �ڽĵ��� ���������� ����
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
					pTargetUI = UIInfo.pUI;		  // ��Ī UI�� �ڽ����� ���°�� �Ļ��Ǵ� �ڽ��� ���̻����
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
