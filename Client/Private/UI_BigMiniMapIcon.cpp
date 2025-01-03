#include "stdafx.h"
#include "..\Public\UI_BigMiniMapIcon.h"

#include "GameInstance.h"

#include "Level_MapTool.h"

_uint CUI_BigMiniMapIcon::m_iNextInstanceId = 0;

CUI_BigMiniMapIcon::CUI_BigMiniMapIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{

}

CUI_BigMiniMapIcon::CUI_BigMiniMapIcon(const CUI_BigMiniMapIcon& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_BigMiniMapIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_BigMiniMapIcon::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		UI_DESC			Desc{};
		Desc.fX = 200;
		Desc.fY = 200;
		Desc.fSizeX = 300;
		Desc.fSizeY = 300;

		Desc.fSpeedPerSec = 10.f;
		Desc.fRotationPerSec = XMConvertToRadians(90.0f);


		if (FAILED(__super::Initialize(&Desc)))
			return E_FAIL;
		if (FAILED(Ready_Components()))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
		if (FAILED(Ready_Components(pArg)))
			return E_FAIL;
	}

	m_iInstanceId = m_iNextInstanceId++;		// ���߿� �ο��� ���̵�� ���Ҽ������� ���� 




	return S_OK;
}

void CUI_BigMiniMapIcon::Priority_Update(_float fTimeDelta)
{
	for (auto pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CUI_BigMiniMapIcon::Update(_float fTimeDelta)
{
	_bool isExcutingEvent = false;
	for (_uint i = 0; i < UI_EVENT_TYPE::MOUSE_END; i++)
	{
		if (true == m_isUIEventType[i])
		{
			switch (i)
			{
			case Engine::CUI::MOUSE_ON:
				ExcuteMouseOn(fTimeDelta);
				isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_OUT:
				ExcuteMouseOut(fTimeDelta);
				isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_LBTN_TAP:
				break;
			case Engine::CUI::MOUSE_LBTN_CLICK:
				ExcuteMouseClick(fTimeDelta);
				isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_END:
				break;
			default:
				break;
			}
		}
	}



	if (nullptr != m_pParentUI)			// �θ� �����Ҷ� �׻� �θ��� ��ġ,������ ���󰡴ٰ� Ư�� Ʈ���Ŷ� �� ������ Ǭ�ٶ�� �����ϱ� 
	{
		if (false == isExcutingEvent)		// �̺�Ʈ ó������ �ƴҶ�����
		{
			_float fParentSizeX = m_pParentUI->Get_fSizeX();
			_float fParentSizeY = m_pParentUI->Get_fSizeY();
	
			m_fX = (m_fOffsetRatioX * fParentSizeX) + m_pParentUI->Get_fX();
			m_fY = (m_fOffsetRatioY * fParentSizeY) + m_pParentUI->Get_fY();
			m_fSizeX = m_fOffsetSizeRatioX * fParentSizeX;
			m_fSizeY = m_fOffsetSizeRatioY * fParentSizeY;
	
			m_fOriginSizeX = m_fSizeX;
			m_fOriginSizeY = m_fSizeY;
		}
	}
	else
	{
		if (false == isExcutingEvent)		// �̺�Ʈ ó������ �ƴҶ�����
		{
			m_fOriginSizeX = m_fSizeX;
			m_fOriginSizeY = m_fSizeY;
		}
	}



	for (auto pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CUI_BigMiniMapIcon::Late_Update(_float fTimeDelta)
{
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
	__super::Late_Update(fTimeDelta);

	if (false == m_isCompleteBatch)
	{
		if (nullptr == m_pParentUI)
			m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);		// �θ� ������ ���� �߰��ϴ°� �ƴ϶� �ֻ��� �θ� �߰��Ѵ�
	}


	for (auto pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CUI_BigMiniMapIcon::Render()
{



	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	if (false == m_isActiveTrigerUV)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_StartUV", &m_vCurStartUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_EndUV", &m_vCurEndUV, sizeof(_float2))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_StartUV", &m_vTrigerTargetStartUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_EndUV", &m_vTrigerTargetEndUV, sizeof(_float2))))
			return E_FAIL;
	}





	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Render_Text();

	// ���� �������� ��ŷUI�� Ȯ���� ���ٶ�� �� UI�� �������� �����ִ°� ������ �ɵ�ʹ�
	// 1. �� UI�� �����ϴ� ��Ʋ���̹����� � ������Ÿ���� ��Ʋ���� �̹�����?
	// 2. �� UI�� �����ϴ� ��Ʋ���̹������� ���� UV�� �����ϰ��ִ�?

	for (auto pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

void CUI_BigMiniMapIcon::Render_Text()
{
	//if (L"" == m_Font.strFontText)
	//	return;




	_int iTextLength = m_Font.strFontText.length();
	_float fTextWidth = 10 * iTextLength;

	_float fFontPosX = m_fX + (m_fSizeX * 0.5f) + 10.f;
	_float fFontPosY = m_fY;
	_float fScale = 0.3f;


	m_pGameInstance->Render_Text(TEXT("Font_145"),
		to_wstring(m_iInstanceId).c_str(), //m_Font.strFontText.c_str(),//
		XMVectorSet(fFontPosX, fFontPosY, 0.f, 1.f),
		XMVectorSet(1.f, 1.f, 0.f, 1.f),//XMLoadFloat4(&m_Font.vFontColor), //XMVectorSet(1.f, 1.f, 0.f, 1.f),//XMLoadFloat4(&m_Font.vFontColor),// // 
		0.f,
		XMVectorSet(0.f, 0.f, 0.f, 1.f),
		fScale);


}


void CUI_BigMiniMapIcon::ChangeTexture(const _wstring& LstrChangeTexturePrototype)
{
	if (FAILED(__super::Delete_Component(TEXT("Com_Texture"))))
	{
		MSG_BOX(L"������Ʈ ���� ����");
		return;
	}

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom);

	m_LstrMyReferenceTexture = LstrChangeTexturePrototype;
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, m_LstrMyReferenceTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		MSG_BOX(L"�ؽ��� ��ü ����");
}


void CUI_BigMiniMapIcon::MouseClick()
{
	if (m_pGameInstance->Get_KeyState(KEY::ALT) == KEY_STATE::HOLD)
	{
		CLevel_MapTool::Set_PickedUI(this);
	}

	if (1 == m_iInstanceId || 2 == m_iInstanceId)
	{

	}




	//switch (m_iInstanceId)
	//{
	//case 13:
	//	m_isUIEventType[UI_EVENT_TYPE::MOUSE_LBTN_CLICK] = true;
	//	break;
	//
	//default:
	//	break;
	//}
}

void CUI_BigMiniMapIcon::MouseOn()
{

}

void CUI_BigMiniMapIcon::MouseOut()
{

}



void CUI_BigMiniMapIcon::ExcuteMouseClick(_float fTimeDelta)
{
	m_pParentUI->SetActivate(false);

	m_isUIEventType[UI_EVENT_TYPE::MOUSE_LBTN_CLICK] = false;
}

void CUI_BigMiniMapIcon::ExcuteMouseOn(_float fTimeDelta)
{

}

void CUI_BigMiniMapIcon::ExcuteMouseOut(_float fTimeDelta)
{

}


HRESULT CUI_BigMiniMapIcon::Ready_Components(void* pArg)
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	if (nullptr == pArg)
		m_LstrMyReferenceTexture = TEXT("Prototype_Component_Texture_UIAtlas_All0");
	else
	{
		UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
		m_LstrMyReferenceTexture = pDesc->LstrTexturePrototype;
	}

	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, m_LstrMyReferenceTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

CUI_BigMiniMapIcon* CUI_BigMiniMapIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_BigMiniMapIcon* pInstance = new CUI_BigMiniMapIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_BigMiniMapIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_BigMiniMapIcon::Clone(void* pArg)
{
	CUI_BigMiniMapIcon* pInstance = new CUI_BigMiniMapIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_BigMiniMapIcon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_BigMiniMapIcon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
