#pragma once

#include "Base.h"
#include "Transform.h"

/* 모든 게임내에 사용되는 게임 오브젝트들의 부모 클래스다. */

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct : public CTransform::TRANSFORM_DESC
	{
		
	} GAMEOBJECT_DESC;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;


public:
	class CTransform* Get_Transform()	{ return m_pTransformCom; }


	void		      Set_PrototypeName(const _wstring& strPrototypeName) { m_LstrPrototypeName = strPrototypeName; }
	const _wstring&	  Get_PrototypeName() { return m_LstrPrototypeName; }

	void		      Set_LayerName(const _wstring& strLayerName) { m_LstrLayerName = strLayerName; }
	const _wstring&   Get_LayerName() { return m_LstrLayerName; }

	


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	
	virtual HRESULT Render();
	virtual HRESULT Render_Height() { return S_OK; }
	virtual HRESULT Render_LightDepth() { return S_OK; }

public:
	virtual class CComponent* Find_Component(const _wstring& strComponentTag, _uint iPartObjIndex = 0);


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	class CTransform*			m_pTransformCom = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };



protected:
	map<const _wstring, class CComponent*>			m_Components;
	_wstring										m_LstrPrototypeName = L"";				// 객체 플토타입이름
	_wstring										m_LstrLayerName = L"";					// 객체 레이어이름


protected:	
	HRESULT Add_Component(_uint iLevelIndex, const _wstring& strPrototypeTag, const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);
	HRESULT Delete_Component(const _wstring& strComponentTag);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END