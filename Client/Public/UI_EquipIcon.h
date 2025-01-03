#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CVIBuffer_RectBatch;
END

BEGIN(Client)

class CUI_EquipIcon final : public CUI
{
public:
	enum
	{
		WEAPON_PANEL,        // 무기패널
		HELMET_PANEL,        // 투구패널
		ARMOR_PANEL,         // 갑옷패널
		CAPE_PANEL,          // 망토패널
		SHOULDER_PANEL,      // 어깨패널
		HAND_PANEL,          // 손패널
		BOOTS_PANEL,         // 신발패널
		WEAPON_ARMOR_PANEL,  // 무기방어구패널
		ACCESSORY_PANEL,     // 악세서리패널
		DUMMY_PANEL1,
		DUMMY_PANEL2,

		WEAPON_ICON,         // 무기아이콘
		HELMET_ICON,         // 투구아이콘
		ARMOR_ICON,          // 갑옷아이콘
		CAPE_ICON,           // 망토아이콘
		SHOULDER_ICON,       // 어깨아이콘
		HAND_ICON,           // 손아이콘
		BOOTS_ICON,          // 신발아이콘
		WEAPON_ARMOR_ICON,   // 무기방어구아이콘
		ACCESSORY_ICON,      // 악세서리아이콘
		DUMMY_ICON1,
		DUMMY_ICON2,

	}TYPE;
	

private:
	CUI_EquipIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_EquipIcon(const CUI_EquipIcon& Prototype);
	virtual ~CUI_EquipIcon() = default;

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
	static CUI_EquipIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END