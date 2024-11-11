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

	/* ���������� ���� �����͵��� ��� ���ϳ�. */
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
	/* ���������� ���� ������ı��� �����ϰ� �ȴ�. */
	//__super::Late_Update(fTimeDelta);

	for (auto& UIInfo : m_UIList)
	{
		UIInfo.pUI->Late_Update(fTimeDelta);

		// ��ġ�� �ֵ��� ���������� ��Ʈ�� �Ϸ��� 
		// �ϴ� , Ű�е� , �޴� , ���������� ���ʴ� 

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


	// ������ ���� ��Ʋ�󽺿� �մ� 1�����ΰ����� ������ ����.. ���̴����� �ؽ��� �迭 ������������ �ȴ�
	if (FAILED(m_pAtlasTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;
	if (FAILED(m_pVIBufferRectBatch->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferRectBatch->Render()))
		return E_FAIL;



	// ���� �������� ��ŷUI�� Ȯ���� ���ٶ�� �� UI�� �������� �����ִ°� ������ �ɵ�ʹ�
	// 1. �� UI�� �����ϴ� ��Ʋ���̹����� � ������Ÿ���� ��Ʋ���� �̹�����?
	// 2. �� UI�� �����ϴ� ��Ʋ���̹������� ���� UV�� �����ϰ��ִ�?

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
	if (FAILED(__super::Add_Component(3, TEXT("Prototype_Component_Shader_VtxPosTex"),		// �̰� ���� �ӽ�.. ����ؾ���
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* FOR.Com_VIBuffer */
	if (FAILED(__super::Add_Component(3, TEXT("Prototype_Component_VIBuffer_RectBatch"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferRectBatch))))
		return E_FAIL;


	// �ϴ� �̸� UI���� �����س��� �� �༮���� ��ġ���� ���� ������ 
	// �׸��� �� ��ġ���������� �׷��� �����ϰ� ������ 
	// ������� Player ������ â UI , Playerü�¿� ���� â UI, �÷��̾� �����̳� ���

	// ���� ��ġ�� �� UI�� ������ġ, ���û���� �־��ְ�, ������ UV�� �־���
	// ������ġ�� ���۰� ���õɰ��̰� ������ UV�� �´� �ؽ��İ� �־������̴�
	// ���� �̰� ��ġ�μ����Ѵٸ� ��ο� ���� 1���� �Ͼ����
	// ���⼭ �ٵ� ��ġ�� �ֵ��� �������� ��Ұ� ����Ǿ��Ѵٸ�  dmf gksk
	// �� Batch UI�� ����� ��ġ�� ©©�� UI�� ������ �����鼭 �갡 ������Ʈ���ش� ?

	// �ϴ� ȭ��� �ִ� UI�� ��� ��ġ�׷쿡 ���´ٴ� �������� ����
	// �����߿� !!!!!!!!!! >>>>>>>>> ��ġ�������� ���ԵǴ¾ֵ��� �������� �ʴ°� (����)�������ΰ���.
	// ��ġ�Ⱦֵ��� �������� �ʴ°� �����ϰ� ����


	list<CGameObject*>& UILists = m_pGameInstance->Get_GameObjects(3, L"Layer_UI_ForBatch");
	m_iCountUI_ForBatch = UILists.size();		// ��ġ�� �������� UI�� ����


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

		// 1. �������� UV�� ����Ǿ��ϰ�,
		// 2. � ���������� UV�� ����Ǿ��ϴ����� üũ�ؾ��Ѵ�.
		// 3. ���������� ũ��,��ġ�� �������� ����ɼ��ִ�.   �̰� ������ġ�� ���ϱ� �׽�Ʈ�غ���

		// �׷� ��ġ�� �������۰� � ������������ ������ �Ǿ��Ѵ�.

		if (L"" != MyReferenceTexture)
		{
			if (MyReferenceTexture != m_LstrMyReferenceTexture)
			{
				MSG_BOX(L"��ġ�� �Ϸ��� ȭ�鿡 ��ġ�� ������UI����\n�ϳ��� ��Ʋ�� �̹����������ؾ���\n���� �ٸ��̹����ϰ�� ��ġ�Ұ���..��");
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
		MSG_BOX(L"��ġ�� �ʿ��� �ؽ��� ���� ����");


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
