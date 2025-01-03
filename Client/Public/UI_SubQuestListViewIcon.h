#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_SubQuestListViewIcon final : public CUI
{
public:
	enum ICON_TYPE
	{
		QUESTPANEL1,
		QUESTPANEL2,
		QUESTPANEL3,

		EXIT,
		HELP_DECO,
		EXIT2,

		QUEST_SCRIPT_PANEL1,
		QUEST_SCRIPT_PANEL2,
		QUEST_SCRIPT_PANEL3,
		QUEST_SCRIPT_PANEL4,
		QUEST_SCRIPT_PANEL5,
		QUEST_SCRIPT_PANEL6,

		QUEST_SCRIPT_CHECK1,
		QUEST_SCRIPT_CHECK2,
		QUEST_SCRIPT_CHECK3,
		QUEST_SCRIPT_CHECK4,
		QUEST_SCRIPT_CHECK5,
		QUEST_SCRIPT_CHECK6,      //  18 ~ 19  ~ 20  데코

		EXP1 = 21,
		EXP2 = 22,
		EXP3 = 23,
		EXP4 = 24,
		EXP5 = 25,
		EXP6 = 26,
		
		BOX = 33,
		SPRINT = 34,



		//35~40 수락
		//41~46 취소

	};


private:
	CUI_SubQuestListViewIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SubQuestListViewIcon(const CUI_SubQuestListViewIcon& Prototype);
	virtual ~CUI_SubQuestListViewIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual	void	Render_Text() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


private:
	_float				 m_fOriginSizeX = {};
	_float				 m_fOriginSizeY = {};

	_float				 m_fAccTime = {};

	static _uint		 m_iNextInstanceId;
	_uint				 m_iInstanceId = 0;

private:
	virtual void	ChangeTexture(const _wstring& LstrChangeTexturePrototype) override;


private:
	virtual void MouseClick() override;
	virtual void MouseOn() override;
	virtual void MouseOut() override;

private:
	virtual void ExcuteMouseClick(_float fTimeDelta);
	virtual void ExcuteMouseOn(_float fTimeDelta);
	virtual void ExcuteMouseOut(_float fTimeDelta);




private:
	HRESULT Ready_Components(void* pArg = nullptr);

public:
	static CUI_SubQuestListViewIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END