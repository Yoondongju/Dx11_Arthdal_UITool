#pragma once

#include "UI.h"
#include "VIBuffer_RectBatch.h"


BEGIN(Engine)

class ENGINE_DLL CUIBatch final : public CUI
{
public:
	enum BATHCING_GROUP
	{
		BACKGROUND_DEFAULT,	
		INVENTORY_ICON,
		MAINMENU_ICON,
		MAINMENU_DECO,

		SKILLMENU_ICON,
		SKILLMENU_DECO,

		PORTIONMENU_ICON,
		PORTIONMENU_DECO,
		PORTIONMENU_DECO_2,
	

		BATHCING_GROUP_END
	};

	typedef struct
	{
		BATHCING_GROUP	eGroup = { BATHCING_GROUP_END };


	}BATCH_GROUP_DESC;

private:
	CUIBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIBatch(const CUIBatch& Prototype);
	virtual ~CUIBatch() = default;

public:
	BATHCING_GROUP							 Get_MyBatchingGroup() { return m_eBatchGroup; }
	_wstring								 Get_MyBatchingGroupName()
	{
		switch (m_eBatchGroup)
		{
		case CUIBatch::BACKGROUND_DEFAULT:
			return L"BACKGROUND_DEFAULT";
			break;
		case CUIBatch::INVENTORY_ICON:
			return L"INVENTORY_ICON";
			break;
		case CUIBatch::MAINMENU_ICON:
			return L"MAINMENU_ICON";
			break;
		case CUIBatch::MAINMENU_DECO:
			return L"MAINMENU_DECO";
			break;
		case CUIBatch::SKILLMENU_ICON:
			return L"SKILLMENU_ICON";
			break;
		case CUIBatch::SKILLMENU_DECO:
			return L"SKILLMENU_DECO";
			break;
		case CUIBatch::PORTIONMENU_ICON:
			return L"PORTIONMENU_ICON";
			break;
		case CUIBatch::PORTIONMENU_DECO:
			return L"PORTIONMENU_DECO";
			break;
		case CUIBatch::PORTIONMENU_DECO_2:
			return L"PORTIONMENU_DECO_2";
			break;

		default:
			break;
		}
	}

	list<CVIBuffer_RectBatch::UIBATCH_INFO>& Get_UIList() { return m_UIList; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pAtlasTextureCom = { nullptr };

	// 어떤 배치
	BATHCING_GROUP												m_eBatchGroup = { BATHCING_GROUP_END };
	list<CVIBuffer_RectBatch::UIBATCH_INFO>						m_UIList;
	_uint														m_iCountUI_ForBatch = 0;
	CVIBuffer_RectBatch* m_pVIBufferRectBatch = { nullptr };




private:
	virtual void MouseClick() override;



private:
	HRESULT Ready_Components();

public:
	static CUIBatch* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END