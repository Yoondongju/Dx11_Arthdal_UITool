#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_FishingFailIcon final : public CUI
{
private:
    CUI_FishingFailIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CUI_FishingFailIcon(const CUI_FishingFailIcon& Prototype);
    virtual ~CUI_FishingFailIcon() = default;

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
    static CUI_FishingFailIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg);
    virtual void Free() override;

};

END