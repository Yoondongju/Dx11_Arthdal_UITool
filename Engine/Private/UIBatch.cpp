#include "..\Public\UIBatch.h"

#include "GameInstance.h"


CUIBatch::CUIBatch(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{

}

CUIBatch::CUIBatch(const CUIBatch& Prototype)
	: CUI{ Prototype }
{
}

HRESULT CUIBatch::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIBatch::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		BATCH_GROUP_DESC* pDesc = static_cast<BATCH_GROUP_DESC*>(pArg);
		m_eBatchGroup = pDesc->eGroup;
	}

	UI_DESC			Desc{};

	Desc.fX = 1.f;
	Desc.fY = 1.f;
	Desc.fSizeX = 1.f;
	Desc.fSizeY = 1.f;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_eUIType = UI_TYPE::BATCHING;

	/* 직교퉁여을 위한 데이터들을 모두 셋하낟. */
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CUIBatch::Priority_Update(_float fTimeDelta)
{
	for (auto& UIInfo : m_UIList)
	{
		UIInfo.pUI->Priority_Update(fTimeDelta);
	}

}

void CUIBatch::Update(_float fTimeDelta)
{
	for (auto& UIInfo : m_UIList)
	{
		UIInfo.pUI->Update(fTimeDelta);
	}

}

void CUIBatch::Late_Update(_float fTimeDelta)
{
	/* 직교투영을 위한 월드행렬까지 셋팅하게 된다. */
	//__super::Late_Update(fTimeDelta);

	for (auto& UIInfo : m_UIList)
	{
		UIInfo.pUI->Late_Update(fTimeDelta);

		// 배치된 애들을 개별적으로 컨트롤 하려면 
		// 일단 , 키패드 , 메뉴 , 아이콘으로 나눳다 

		const _float4x4* pWorldMatrix = UIInfo.pUI->Get_Transform()->Get_WorldMatrix_Ptr();
		_float2 vStartUV = UIInfo.pUI->Get_StartUV();
		_float2 vEndUV = UIInfo.pUI->Get_EndUV();

		m_pVIBufferRectBatch->Update_UIVertex(pWorldMatrix, UIInfo.iUIIndex, vStartUV, vEndUV);
	}


	if (nullptr == m_pParentUI)
		m_pGameInstance->Add_RenderObject(CRenderer::RG_UI, this);
}

HRESULT CUIBatch::Render()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;


	// 무조건 같은 아틀라스에 잇는 1장으로가야해 여러장 실패.. 셰이더에서 텍스쳐 배열 랜덤엑세스가 안대
	if (FAILED(m_pAtlasTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;
	if (FAILED(m_pVIBufferRectBatch->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferRectBatch->Render()))
		return E_FAIL;



	// 만약 맵툴에서 피킹UI로 확정이 났다라면 이 UI의 정보들을 보여주는게 도움이 될듯싶다
	// 1. 이 UI가 참조하는 아틀라스이미지는 어떤 프로토타입의 아틀라스의 이미지니?
	// 2. 이 UI가 참조하는 아틀라스이미지에서 몇의 UV를 참조하고있니?

	for (auto& UIInfo : m_UIList)
	{
		UIInfo.pUI->Render_Text();
	}



	return S_OK;
}





void CUIBatch::MouseClick()
{

}


HRESULT CUIBatch::Ready_Components()
{
	/* FOR.Com_Shader */
	if (FAILED(__super::Add_Component(3, TEXT("Prototype_Component_Shader_VtxPosTex"),		// 이거 레벨 임시.. 고민해야함
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(3, TEXT("Prototype_Component_VIBuffer_RectBatch"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferRectBatch))))
		return E_FAIL;


	// 일단 미리 UI들을 세팅해놓고 그 녀석들을 배치할지 말지 정하자 
	// 그리고 이 배치렌더링을할 그룹을 세밀하게 나누자 
	// 예를들어 Player 스탯의 창 UI , Player체력에 대한 창 UI, 플레이어 레벨이나 등등

	// 내가 배치를 한 UI의 로컬위치, 로컬사이즈를 넣어주고, 추출한 UV도 넣어줌
	// 로컬위치의 버퍼가 세팅될것이고 추출한 UV에 맞는 텍스쳐가 넣어질것이다
	// 이제 이걸 배치로세팅한다면 드로우 콜이 1번만 일어나겠지
	// 여기서 근데 배치된 애들의 개별적인 요소가 변경되야한다면  dmf gksk
	// 이 Batch UI는 멤버로 배치된 짤짤이 UI를 가지고 있으면서 얘가 업데이트해준다 ?

	// 일단 화면상에 있는 UI를 모두 배치그룹에 묶는다는 느낌으로 가자
	// 제일중요 !!!!!!!!!! >>>>>>>>> 배치렌더링에 포함되는애들은 움직이지 않는걸 (정적)고정으로간다.
	// 배치된애들은 움직이지 않는걸 가정하고 간다


	list<CGameObject*>& UILists = m_pGameInstance->Get_GameObjects(3, L"Layer_UI_ForBatch");
	m_iCountUI_ForBatch = UILists.size();		// 배치될 독립적인 UI의 갯수


	_uint		iIndex = 0;
	static _wstring MyReferenceTexture = L"";

	for (auto UI : UILists)
	{
		CUI* pUI = static_cast<CUI*>(UI);
		_float4x4* pWorldMatrix = const_cast<_float4x4*>(pUI->Get_Transform()->Get_WorldMatrix_Ptr());

		_float fLocalfX = 0.f;
		_float fLocalfY = 0.f;
		_float fLocalfSizeX = 1.f;
		_float fLocalfSizeY = 1.f;
		_wstring LstrTexturePrototypeName = pUI->Get_MyTexturePrototypeName();

		_float2 UVStart = pUI->Get_StartUV();
		_float2 UVEnd = pUI->Get_EndUV();


		m_pVIBufferRectBatch->AddUIQuad(pWorldMatrix, fLocalfX, fLocalfY, fLocalfSizeX, fLocalfSizeY, UVStart, UVEnd);

		// 1. 동적으로 UV가 변경되야하고,
		// 2. 어떤 정점버퍼의 UV가 변경되야하는지를 체크해야한다.
		// 3. 정점버퍼의 크기,위치가 동적으로 변경될수있다.   이건 월드위치로 가니까 테스트해보장

		// 그럼 배치된 정점버퍼가 어떤 정점버퍼인지 구별이 되야한다.

		if (L"" != MyReferenceTexture)
		{
			if (MyReferenceTexture != m_LstrMyReferenceTexture)
			{
				MSG_BOX(L"배치를 하려면 화면에 배치된 각각의UI들이\n하나의 아틀라스 이미지를참조해야함\n서로 다른이미지일경우 배치불가능..ㅠ");
				assert(nullptr);
			}
		}
		m_LstrMyReferenceTexture = LstrTexturePrototypeName;
		MyReferenceTexture = m_LstrMyReferenceTexture;


		pUI->Set_CompleteBatch();
		CVIBuffer_RectBatch::UIBATCH_INFO Info = {};
		Info.iUIIndex = iIndex * 4;
		Info.pUI = pUI;
		m_UIList.emplace_back(Info);
		++iIndex;
	}

	MyReferenceTexture = L"";
	m_pGameInstance->RemoveAllObjectToLayer(3, L"Layer_UI_ForBatch");
	m_pVIBufferRectBatch->Initialize_Batch(nullptr);


	if (FAILED(__super::Add_Component(3, m_LstrMyReferenceTexture,
		L"Com_Texture", reinterpret_cast<CComponent**>(&m_pAtlasTextureCom))))
		MSG_BOX(L"배치에 필요한 텍스쳐 생성 실패");


	return S_OK;
}

CUIBatch* CUIBatch::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUIBatch* pInstance = new CUIBatch(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CUIBatch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject* CUIBatch::Clone(void* pArg)
{
	CUIBatch* pInstance = new CUIBatch(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CUIBatch"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIBatch::Free()
{
	__super::Free();

	for (auto& pUI : m_UIList)
	{
		Safe_Release(pUI.pUI);
	}
	m_UIList.clear();


	Safe_Release(m_pShaderCom);

	Safe_Release(m_pAtlasTextureCom);

	Safe_Release(m_pVIBufferRectBatch);
}
