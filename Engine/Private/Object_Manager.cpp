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

	/* 복제해야할 원형을 차즌ㄴ다. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 그 원형을 복제하여 사본 객체를 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	/* 객체들은 레잉어로 묶어서 관리하고 있었거든 */
	/* 사본을 추가하기위한 레이어를 찾자.*/
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 야 그 레이어가 없는디? */
	/* 내가 추가하려고했던 레이어가 아직없었다 == 처음 추가하는 객체였다. */
	/* 새롱누 렝이ㅓ를 만들어야지. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);		
	}
	else /* 내가 추가하려고 하는 레잉어가 만들어져있었어. */
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

	/* 야 그 레이어가 없는디? */
	/* 내가 추가하려고했던 레이어가 아직없었다 == 처음 추가하는 객체였다. */
	/* 새롱누 렝이ㅓ를 만들어야지. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pObj);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else /* 내가 추가하려고 하는 레잉어가 만들어져있었어. */
		pLayer->Add_GameObject(pObj);


	pObj->Set_LayerName(strLayerTag);
	return S_OK;
}

HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
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
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
			Pair.second->Update(fTimeDelta);
	}

	return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
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
		MSG_BOX(L"검색한 레이어가 존재하지않아요");
		return;
	}

	return pLayer->RemoveObjectToLayer(pGameObject);
}

void CObject_Manager::RemoveAllObjectToLayer(_uint iLevelIndex, const _wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		MSG_BOX(L"검색한 레이어가 존재하지않아요");
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
