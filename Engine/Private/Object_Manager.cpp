#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
	{
		Safe_Release(pPrototype);
		return E_FAIL;
	}

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	/* �����ؾ��� ������ ���𤤴�. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* �� ������ �����Ͽ� �纻 ��ü�� �����Ѵ�. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	/* ��ü���� ���׾�� ��� �����ϰ� �־��ŵ� */
	/* �纻�� �߰��ϱ����� ���̾ ã��.*/
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* �� �� ���̾ ���µ�? */
	/* ���� �߰��Ϸ����ߴ� ���̾ ���������� == ó�� �߰��ϴ� ��ü����. */
	/* ���մ� ���̤ø� ��������. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);		
	}
	else /* ���� �߰��Ϸ��� �ϴ� ���׾ ��������־���. */
		pLayer->Add_GameObject(pGameObject);


	pGameObject->Set_PrototypeName(strPrototypeTag);
	pGameObject->Set_LayerName(strLayerTag);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pObj)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* �� �� ���̾ ���µ�? */
	/* ���� �߰��Ϸ����ߴ� ���̾ ���������� == ó�� �߰��ϴ� ��ü����. */
	/* ���մ� ���̤ø� ��������. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pObj);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else /* ���� �߰��Ϸ��� �ϴ� ���׾ ��������־���. */
		pLayer->Add_GameObject(pObj);


	pObj->Set_LayerName(strLayerTag);
	return S_OK;
}

HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Priority_Update(fTimeDelta);	
		}	
	}
	return S_OK;
}

HRESULT CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}

	return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC�� ���̾��� ���� �Ҵ�� ������ ���̾�鸸 ��ȿ�ϰ� ����ִ� ��Ȳ�� �ɲ���. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Late_Update(fTimeDelta);
	}

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CGameObject * CObject_Manager::Clone_GameObject(const _wstring & strPrototypeTag, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	pGameObject->Set_PrototypeName(strPrototypeTag);

	return pGameObject;
}

CComponent * CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComponentTag, iIndex, iPartObjIndex);	
}

list<class CGameObject*>& CObject_Manager::Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag)
{
	static list<class CGameObject*> emptyList;

	CLayer* pLayer = Find_Layer(iLevelIndx, strLayerTag);
	if (nullptr == pLayer)
		return emptyList;


	return pLayer->Get_GamObjects();
}

CGameObject* CObject_Manager::Find_Camera()
{
	CGameObject* pCamera = Find_Object(1, TEXT("Layer_Camera"), 0);
	if (nullptr == pCamera)
		return nullptr;

	return pCamera;
}


CLayer* CObject_Manager::Delete_Layer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
	{
		assert(nullptr);
		return nullptr;
	}

	if (0 < iter->second->Get_GamObjectSize())
		return nullptr;


	CLayer* pTempLayer = iter->second;


	m_pLayers[iLevelIndex].erase(iter->first);


	return pTempLayer;
}

void CObject_Manager::RemoveObjectToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		MSG_BOX(L"�˻��� ���̾ ���������ʾƿ�");
		return;
	}

	return pLayer->RemoveObjectToLayer(pGameObject);
}

void CObject_Manager::RemoveAllObjectToLayer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		MSG_BOX(L"�˻��� ���̾ ���������ʾƿ�");
		return;
	}

	return pLayer->RemoveAllObjectToLayer();
}


CGameObject* CObject_Manager::Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Object(iIndex);
}

CGameObject * CObject_Manager::Find_Prototype(const _wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);
	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring & strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);
		m_pLayers[i].clear();		
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}
