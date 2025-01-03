#include "stdafx.h"
#include "..\Public\UI_DuacaEscapeKey.h"

#include "GameInstance.h"

#include "Level_MapTool.h"

CUI_DuacaEscapeKey::CUI_DuacaEscapeKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{

}

CUI_DuacaEscapeKey::CUI_DuacaEscapeKey(const CUI_DuacaEscapeKey& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_DuacaEscapeKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_DuacaEscapeKey::Initialize(void* pArg)
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


	m_Font.strFontText = L"Test: 720 / 720";
	m_Font.vFontColor = _float4(1.f, 1.f, 1.f, 1.f);

	return S_OK;
}

void CUI_DuacaEscapeKey::Priority_Update(_float fTimeDelta)
{
	for (auto pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CUI_DuacaEscapeKey::Update(_float fTimeDelta)
{
	m_fAccTime += fTimeDelta;
	m_bActivate = true;

	_bool isExcutingEvent = false;
	for (_uint i = 0; i < UI_EVENT_TYPE::MOUSE_END; i++)
	{
		if (true == m_isUIEventType[i])
		{
			switch (i)
			{
			case Engine::CUI::MOUSE_ON:
				//ExcuteMouseOn(fTimeDelta);
				//isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_OUT:
				//ExcuteMouseOut(fTimeDelta);
				//isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_LBTN_TAP:
				break;
			case Engine::CUI::MOUSE_LBTN_CLICK:
				//ExcuteMouseClick(fTimeDelta);
				//isExcutingEvent = true;
				break;
			case Engine::CUI::MOUSE_END:
				break;
			default:
				break;
			}
		}
	}


	if (nullptr != m_pParentUI)			// 부모가 존재할때 항상 부모의 위치,스케일 따라가다가 특정 트리거때 이 조건을 푼다라는 생각하기 
	{
		if (false == isExcutingEvent)		// 이벤트 처리중이 아닐떼ㅐ만
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
		if (false == isExcutingEvent)		// 이벤트 처리중이 아닐떼ㅐ만
		{
			m_fOriginSizeX = m_fSizeX;
			m_fOriginSizeY = m_fSizeY;
		}
	}



	for (auto pChildUI : m_childUI_List)
		pChildUI->Update(fTimeDelta);
}

void CUI_DuacaEscapeKey::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	__super::Late_Update(fTimeDelta);

	if (false == m_isCompleteBatch)
	{
		if (nullptr == m_pParentUI)
			m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);		// 부모가 잇으면 직접 추가하는게 아니라 최상위 부모만 추가한다
	}

	for (auto pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);

	
}

HRESULT CUI_DuacaEscapeKey::Render()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartUV", &m_vCurStartUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndUV", &m_vCurEndUV, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fAccTime, sizeof(_float))))
		return E_FAIL;




	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	Render_Text();

	// 만약 맵툴에서 피킹UI로 확정이 났다라면 이 UI의 정보들을 보여주는게 도움이 될듯싶다
	// 1. 이 UI가 참조하는 아틀라스이미지는 어떤 프로토타입의 아틀라스의 이미지니?
	// 2. 이 UI가 참조하는 아틀라스이미지에서 몇의 UV를 참조하고있니?

	for (auto pChildUI : m_childUI_List)
		pChildUI->Render();

	return S_OK;
}

void CUI_DuacaEscapeKey::Render_Text()
{
	_float fFontPosX = m_fX - (m_fSizeX * 0.5f) + 10.f;
	_float fFontPosY = m_fY - (m_fSizeY * 0.5f);

	m_pGameInstance->Render_Text(TEXT("Font_145"),
		m_Font.strFontText.c_str(),
		XMVectorSet(fFontPosX, fFontPosY, 0.f, 1.f),
		XMLoadFloat4(&m_Font.vFontColor),
		0.f,
		XMVectorSet(0.f, 0.f, 0.f, 1.f),
		0.35f);




	//if (CLevel_MapTool::Get_PickedUI() == this)
	//{
	//	_wstring LMyReferenceTexture =
	//		L"My Prototype:  " + m_LstrPrototypeName + L"\n"
	//		+ L"My Layer:  " + m_LstrLayerName;
	//
	//
	//	_wstring LMyReferenceTexture2 =
	//		L"MyTexture:  " + m_LstrMyReferenceTexture + L"\n"
	//		+ L"StartUV.x:  " + to_wstring(m_vCurStartUV.x) + L"\n"
	//		+ L"StartUV.y:  " + to_wstring(m_vCurStartUV.y) + L"\n"
	//		+ L"EndUV.x:  " + to_wstring(m_vCurEndUV.x) + L"\n"
	//		+ L"EndUV.y:  " + to_wstring(m_vCurEndUV.y);
	//
	//	_float fTextPosX = m_fX + m_fSizeX * 0.5f;
	//	_float fTextPosY = m_fY - m_fSizeY * 0.5f;
	//
	//	m_pGameInstance->Render_Text(TEXT("Font_145"),
	//		LMyReferenceTexture.c_str(),
	//		XMVectorSet(fTextPosX, fTextPosY, 0.f, 1.f),
	//		XMVectorSet(1.f, 1.f, 0.f, 1.f),
	//		0.f,
	//		XMVectorSet(0.f, 0.f, 0.f, 1.f),
	//		0.35f);
	//
	//	fTextPosX = m_fX + m_fSizeX * 0.5f;
	//	fTextPosY = m_fY + m_fSizeY * 0.2f;
	//
	//	m_pGameInstance->Render_Text(TEXT("Font_145"),
	//		LMyReferenceTexture2.c_str(),
	//		XMVectorSet(fTextPosX, fTextPosY, 0.f, 1.f),
	//		XMVectorSet(1.f, 1.f, 0.f, 1.f),
	//		0.f,
	//		XMVectorSet(0.f, 0.f, 0.f, 1.f),
	//		0.35f);
	//}
}


void CUI_DuacaEscapeKey::ChangeTexture(const _wstring& LstrChangeTexturePrototype)
{
	if (FAILED(__super::Delete_Component(TEXT("Com_Texture"))))
	{
		MSG_BOX(L"컴포넌트 삭제 실패");
		return;
	}

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom);

	m_LstrMyReferenceTexture = LstrChangeTexturePrototype;
	if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, m_LstrMyReferenceTexture,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		MSG_BOX(L"텍스쳐 교체 실패");
}


void CUI_DuacaEscapeKey::MouseClick()
{
	if (m_pGameInstance->Get_KeyState(KEY::ALT) == KEY_STATE::HOLD)
	{
		CLevel_MapTool::Set_PickedUI(this);
	}


	m_isUIEventType[UI_EVENT_TYPE::MOUSE_LBTN_CLICK] = true;
}

void CUI_DuacaEscapeKey::MouseOn()
{

}

void CUI_DuacaEscapeKey::MouseOut()
{

}



void CUI_DuacaEscapeKey::ExcuteMouseClick(_float fTimeDelta)
{
}

void CUI_DuacaEscapeKey::ExcuteMouseOn(_float fTimeDelta)
{

}

void CUI_DuacaEscapeKey::ExcuteMouseOut(_float fTimeDelta)
{

}


HRESULT CUI_DuacaEscapeKey::Ready_Components(void* pArg)
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

CUI_DuacaEscapeKey* CUI_DuacaEscapeKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_DuacaEscapeKey* pInstance = new CUI_DuacaEscapeKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_DuacaEscapeKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_DuacaEscapeKey::Clone(void* pArg)
{
	CUI_DuacaEscapeKey* pInstance = new CUI_DuacaEscapeKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_DuacaEscapeKey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_DuacaEscapeKey::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
