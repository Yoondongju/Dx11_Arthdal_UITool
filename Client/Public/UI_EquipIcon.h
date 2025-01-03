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
		WEAPON_PANEL,        // �����г�
		HELMET_PANEL,        // �����г�
		ARMOR_PANEL,         // �����г�
		CAPE_PANEL,          // �����г�
		SHOULDER_PANEL,      // ����г�
		HAND_PANEL,          // ���г�
		BOOTS_PANEL,         // �Ź��г�
		WEAPON_ARMOR_PANEL,  // ������г�
		ACCESSORY_PANEL,     // �Ǽ������г�
		DUMMY_PANEL1,
		DUMMY_PANEL2,

		WEAPON_ICON,         // ���������
		HELMET_ICON,         // ����������
		ARMOR_ICON,          // ���ʾ�����
		CAPE_ICON,           // ���������
		SHOULDER_ICON,       // ���������
		HAND_ICON,           // �վ�����
		BOOTS_ICON,          // �Ź߾�����
		WEAPON_ARMOR_ICON,   // �����������
		ACCESSORY_ICON,      // �Ǽ�����������
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