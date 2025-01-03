#include "stdafx.h"
#include "..\Public\UI_MainMenuPanel.h"

#include "GameInstance.h"

#include "Level_MapTool.h"


CUI_MainMenuPanel::CUI_MainMenuPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{

}

CUI_MainMenuPanel::CUI_MainMenuPanel(const CUI_MainMenuPanel& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUI_MainMenuPanel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_MainMenuPanel::Initialize(void* pArg)
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

	//m_fSizeX = 1000.f;
	//m_fSizeY = 900.f;


	return S_OK;
}

void CUI_MainMenuPanel::Priority_Update(_float fTimeDelta)
{
	//m_bActivate = true;

	//m_fSizeX = 500.f;
	//m_fSizeY = 500.f;
	//m_fX = 800.f;
	//m_fY = 450.f;

	if (false == m_bActivate)
		return;

	for (auto pChildUI : m_childUI_List)
		pChildUI->Priority_Update(fTimeDelta);
}

void CUI_MainMenuPanel::Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	m_fAccTime += fTimeDelta;

	

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

void CUI_MainMenuPanel::Late_Update(_float fTimeDelta)
{
	if (false == m_bActivate)
		return;

	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	__super::Late_Update(fTimeDelta);

	if (false == m_isCompleteBatch)
	{
		if (nullptr == m_pParentUI)
			m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);		// 부모가 잇으면 직접 추가하는게 아니라 최상위 부모만 추가한다
	}

	for (auto pChildUI : m_childUI_List)
		pChildUI->Late_Update(fTimeDelta);
}

HRESULT CUI_MainMenuPanel::Render()
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




	if (FAILED(m_pShaderCom->Begin(4)))
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


	m_bActivate = false;			// 한번 m_fX 랑 m_fY가 세팅된후에 꺼주자 자꾸 클라파싱하면 없어진다.

	return S_OK;
}

void CUI_MainMenuPanel::Render_Text()
{
	if (CLevel_MapTool::Get_PickedUI() == this)
	{
		_wstring LMyReferenceTexture =
			L"My Prototype:  " + m_LstrPrototypeName + L"\n"
			+ L"My Layer:  " + m_LstrLayerName;


		_wstring LMyReferenceTexture2 =
			L"MyTexture:  " + m_LstrMyReferenceTexture + L"\n"
			+ L"StartUV.x:  " + to_wstring(m_vCurStartUV.x) + L"\n"
			+ L"StartUV.y:  " + to_wstring(m_vCurStartUV.y) + L"\n"
			+ L"EndUV.x:  " + to_wstring(m_vCurEndUV.x) + L"\n"
			+ L"EndUV.y:  " + to_wstring(m_vCurEndUV.y);

		_float fTextPosX = m_fX + m_fSizeX * 0.5f;
		_float fTextPosY = m_fY - m_fSizeY * 0.5f;

		m_pGameInstance->Render_Text(TEXT("Font_145"),
			LMyReferenceTexture.c_str(),
			XMVectorSet(fTextPosX, fTextPosY, 0.f, 1.f),
			XMVectorSet(1.f, 1.f, 0.f, 1.f),
			0.f,
			XMVectorSet(0.f, 0.f, 0.f, 1.f),
			0.35f);

		fTextPosX = m_fX + m_fSizeX * 0.5f;
		fTextPosY = m_fY + m_fSizeY * 0.2f;

		m_pGameInstance->Render_Text(TEXT("Font_145"),
			LMyReferenceTexture2.c_str(),
			XMVectorSet(fTextPosX, fTextPosY, 0.f, 1.f),
			XMVectorSet(1.f, 1.f, 0.f, 1.f),
			0.f,
			XMVectorSet(0.f, 0.f, 0.f, 1.f),
			0.35f);
	}
}


void CUI_MainMenuPanel::ChangeTexture(const _wstring& LstrChangeTexturePrototype)
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


void CUI_MainMenuPanel::MouseClick()
{
	if (m_pGameInstance->Get_KeyState(KEY::ALT) == KEY_STATE::HOLD)
	{
		CLevel_MapTool::Set_PickedUI(this);
	}


	m_isUIEventType[UI_EVENT_TYPE::MOUSE_LBTN_CLICK] = true;
}

void CUI_MainMenuPanel::MouseOn()
{

}

void CUI_MainMenuPanel::MouseOut()
{

}



void CUI_MainMenuPanel::ExcuteMouseClick(_float fTimeDelta)
{
	m_isUIEventType[UI_EVENT_TYPE::MOUSE_LBTN_CLICK] = false;
}


void CUI_MainMenuPanel::ExcuteMouseOn(_float fTimeDelta)
{

}

void CUI_MainMenuPanel::ExcuteMouseOut(_float fTimeDelta)
{

}


HRESULT CUI_MainMenuPanel::Ready_Components(void* pArg)
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

CUI_MainMenuPanel* CUI_MainMenuPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_MainMenuPanel* pInstance = new CUI_MainMenuPanel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUI_MainMenuPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUI_MainMenuPanel::Clone(void* pArg)
{
	CUI_MainMenuPanel* pInstance = new CUI_MainMenuPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUI_MainMenuPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_MainMenuPanel::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
