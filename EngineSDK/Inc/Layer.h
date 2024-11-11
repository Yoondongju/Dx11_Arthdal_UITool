#pragma once

#include "Base.h"

/* 객체들을 보관한다. */

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;


public:
	_uint Get_GamObjectSize();
	list<class CGameObject*>& Get_GamObjects() { return m_GameObjects; }


public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);

public:
	class CComponent*  Find_Component(const _wstring& strComponentTag, _uint iIndex, _uint iPartObjIndex);
	class CGameObject* Find_Object(_uint iIndex);

	void			   RemoveObjectToLayer(CGameObject* pGameObject);		// 오해금지 단순 레이어에서 빼는거지 그 오브젝트를 삭제하는건아님 그래서 잘 써야해 릭난다 잘못하면 
	void			   RemoveAllObjectToLayer();							// 오해금지   레이어에 있는 모든 오브젝트를 clear
	

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END