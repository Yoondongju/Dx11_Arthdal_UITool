#pragma once

#include "Base.h"

/* ��ü���� �����Ѵ�. */

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

	void			   RemoveObjectToLayer(CGameObject* pGameObject);		// ���ر��� �ܼ� ���̾�� ���°��� �� ������Ʈ�� �����ϴ°Ǿƴ� �׷��� �� ����� ������ �߸��ϸ� 
	void			   RemoveAllObjectToLayer();							// ���ر���   ���̾ �ִ� ��� ������Ʈ�� clear
	

private:
	list<class CGameObject*>			m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END