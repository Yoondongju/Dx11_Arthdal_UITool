#include "../Public/Event_Manager.h"
#include "../Public/GameObject.h"

#include "../Public/Level_Manager.h"
#include "../Public/Object_Manager.h"
#include "../Public/UI_Manager.h"

#include "../Public/Level.h"
#include "../Public/Layer.h"

CEvent_Manager::CEvent_Manager(CObject_Manager* pObject_Manager)
	: m_pObject_Manager{ pObject_Manager }
{
	Safe_AddRef(m_pObject_Manager);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

void CEvent_Manager::update()
{
	for (auto Event = m_Eventlist.begin(); Event != m_Eventlist.end();)
	{
		Excute(*Event);

		Event = m_Eventlist.erase(Event);
	}
	m_Eventlist.clear();
}



void CEvent_Manager::Excute(const EVENT tEvent)
{
	switch ((UINT)tEvent.eEven)
	{
	case (UINT)EVENT_TYPE::CREATE_OBJECT:
	{
		// lParam : �Ⱦ�
		// wParam : �Ⱦ�
		// pArg   : Clone_Desc ����
		//const CLONE_DESC* CloneDesc = tEvent.CloneDesc;

		//m_pObject_Manager->Add_CloneObject_ToLayer(CloneDesc->iLevelIndex, CloneDesc->strLayerTag, CloneDesc->strPrototypeTag, CloneDesc->pArg);
	}
	break;

	case (UINT)EVENT_TYPE::DELETE_OBJECT:
	{
		CGameObject* pDeadObj = (CGameObject*)tEvent.lParam;
		_uint		 iLevelIndex = (_uint)tEvent.wParam;


		_wstring LayerTag = pDeadObj->Get_LayerName();
		list<CGameObject*>& GameObjectList = m_pObject_Manager->Get_GameObjects(iLevelIndex, LayerTag);

		CLayer* pLayer = nullptr;

		for (auto iter = GameObjectList.begin(); iter != GameObjectList.end();)
		{
			if (*iter == pDeadObj) // �ܼ� ��ü�� �ּ� ��
			{
				iter = GameObjectList.erase(iter);			// ��ü�� ���� �����ʰ� ����Ʈ���� ���ֱ⸸ �Ұž�

				pLayer = m_pObject_Manager->Delete_Layer(iLevelIndex, LayerTag);

			}
			else
				++iter;
		}
		Safe_Release(pDeadObj);
		Safe_Release(pLayer);
		CUI_Manager::ResetFocusUI();
	}
	break;


	case (UINT)EVENT_TYPE::CHANGE_LEVEL:
	{
		// lParam:   Level Index
		// wParam :  Next Scene Type
		//m_pLevel_Manager->Change_Level((_uint)tEvent.lParam, (CLevel*)tEvent.wParam);
	}
	break;

	case (UINT)EVENT_TYPE::CHANGE_STATE:
	{
		// lParam : AI
		// wParam : Next Type

	}
	break;
	}
}



CEvent_Manager* CEvent_Manager::Create(CObject_Manager* pObject_Manager)
{
	CEvent_Manager* pInstance = new CEvent_Manager(pObject_Manager);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CEvent_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	__super::Free();


	Safe_Release(m_pObject_Manager);

	for (auto pDeadObject : m_Deadlist)
		Safe_Release(pDeadObject);
	m_Deadlist.clear();

	m_Eventlist.clear();
}



