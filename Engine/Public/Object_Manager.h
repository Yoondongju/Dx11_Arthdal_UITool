#pragma once

#include "Base.h"

/* ��ü���� ��Ƽ� �׷�(Layer)���� �����Ѵ�. */
/* ��� ���� ��ü���� �ݺ����� Update�� ȣ�����ش�. */
/* ��� ���� ��ü���� �ݺ����� Render�� ȣ�����ش�.(x) : ��ü���� �׸��� ������ ���� ���� �� ������ �ؾ��� �ʿ䰡 �ִ�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);	
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg);
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, CGameObject* pObj);



	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public:
	class CGameObject* Clone_GameObject(const _wstring& strPrototypeTag, void* pArg = nullptr);
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);

	list<class CGameObject*>& Get_GameObjects(_uint iLevelIndx, const _wstring& strLayerTag);

	class CGameObject* Find_Object(_uint iLevelIndex, const _wstring& strLayerTag, _uint iIndex);
	class CGameObject* Find_Camera();


public:
	class CLayer* CObject_Manager::Delete_Layer(_uint iLevelIndex, const _wstring& strLayerTag);
	void						   RemoveObjectToLayer(_uint iLevelIndex ,const _wstring& strLayerTag , CGameObject* pGameObject);
	void						   RemoveAllObjectToLayer(_uint iLevelIndex, const _wstring& strLayerTag);


private:

	/* ������ü���� �����Ѵ�. */
	map<const _wstring, class CGameObject*>		m_Prototypes;

	/* ��������, �纻��ü���� �׷캰�� ��Ƽ� �����Ѵ�. */
	_uint										m_iNumLevels = {};
	map<const _wstring, class CLayer*>*			m_pLayers = { nullptr };
	typedef map<const _wstring, class CLayer*>		LAYERS;

private:
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END