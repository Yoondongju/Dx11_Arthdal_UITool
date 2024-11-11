#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

_uint CLayer::Get_GamObjectSize()
{
	return m_GameObjects.size();
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Priority_Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Late_Update(fTimeDelta);

	return S_OK;
}

CComponent * CLayer::Find_Component(const _wstring & strComponentTag, _uint iIndex, _uint iPartObjIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag, iPartObjIndex);	
}

CGameObject* CLayer::Find_Object(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter);
}

void CLayer::RemoveObjectToLayer(CGameObject* pGameObject)
{
	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (*iter == pGameObject)
		{
			m_GameObjects.erase(iter);
			break;
		}
		else
			++iter;
	}
}

void CLayer::RemoveAllObjectToLayer()
{
	m_GameObjects.clear();
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}

