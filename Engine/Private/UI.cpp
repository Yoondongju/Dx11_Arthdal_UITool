#include "../Public/UI.h"

#include "../Public/GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice,pContext }
{
}

CUI::CUI(const CGameObject& Prototype)
	:CGameObject{ Prototype }
{
}



HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	/* 유아이의 초기 위치와 사이즈를 설정한다. */
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;
	m_vCurStartUV = pDesc->vStartUV;
	m_vCurEndUV = pDesc->vEndUV;
	m_vTrigerTargetStartUV = pDesc->vTrigerTargetStartUV;
	m_vTrigerTargetEndUV = pDesc->vTrigerTargetEndUV;

	m_Font.strFontText = pDesc->FontInfo.strFontText;
	m_Font.vFontColor = pDesc->FontInfo.vFontColor;

	_uint		iNumView = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumView, &ViewportDesc);

	/* m_ViewMatrix : XMFLOAT4X4 */
	/* XMMatrixIdentity함수의 리턴값 XMMATRIX */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_fViewWidth = ViewportDesc.Width;
	m_fViewHeight = ViewportDesc.Height;

	// XMVector3Equal(XMLoadFloat3(&m_vTmp), XMLoadFloat3(&m_vTmp));	

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	return S_OK;
}

void CUI::Priority_Update(_float fTimeDelta)
{
}

void CUI::Update(_float fTimeDelta)
{
	
}

void CUI::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드 행렬을 만들어 준다. */
	
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_fX - m_fViewWidth * 0.5f, -m_fY + m_fViewHeight * 0.5f, 0.f, 1.f));

	MouseOnCheck();
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::MouseOnCheck()
{
	if (true == m_bPreMouseOn &&
		false == m_bMouseOn)
	{
		MouseOut();
	}
	if(true == m_bMouseOn)
		MouseOn();

	m_bPreMouseOn = m_bMouseOn;

	_float2 vMousePos = m_pGameInstance->Get_MosePos();	
	
	RECT tUIRect;
	SetRect(&tUIRect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f,
		m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);

	if (vMousePos.x > tUIRect.left
		&& vMousePos.x < tUIRect.right
		&& vMousePos.y > tUIRect.top
		&& vMousePos.y < tUIRect.bottom)
	{
		m_bMouseOn = true;
	}
	else
		m_bMouseOn = false;	
}




void CUI::Free()
{
	__super::Free();

	// 내가 만약 부모가 있었더라면 
	// 부모포인터-> null해줘야하고
	// 부모가 가지고잇는 자식리스트에서 빼줘야한다 

	for (auto& pUI : m_childUI_List)
	{
		pUI->SetParent(nullptr);
		Safe_Release(pUI);
	}
	m_childUI_List.clear();


	if (nullptr != m_pParentUI)
	{
		list<CUI*>& ChildUIList = m_pParentUI->GetChildUI();
		for (auto iter = ChildUIList.begin(); iter != ChildUIList.end();)    // 대부분의 C++ 코드에서는 반복자를 복사하여 사용합니다. 이는 표준 관행
		{
			if (this == *iter)
			{
				iter = ChildUIList.erase(iter);
				break;
			}
			else
				++iter;
		}
		m_pParentUI = nullptr;
	}
}
