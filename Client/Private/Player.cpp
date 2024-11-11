#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "Particle_Explosion.h"
#include "Body_Player.h"
#include "Weapon.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CContainerObject { pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & Prototype)
	: CContainerObject { Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC		Desc{};

	Desc.fSpeedPerSec = 10.0f;
	Desc.fRotationPerSec = XMConvertToRadians(180.0f);

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	m_iState = STATE_IDLE;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.1f, 0.f, 0.1f, 1.f));

	return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{


	if (GetKeyState(VK_DOWN) & 0x8000)
		m_pTransformCom->Go_Backward(fTimeDelta);

	if (GetKeyState(VK_LEFT) & 0x8000)
		m_pTransformCom->Turn(false, true, false, fTimeDelta * -1.f);

	if (GetKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Turn(false, true, false, fTimeDelta);

	if (GetKeyState(VK_UP) & 0x8000)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);

		if(m_iState & STATE_IDLE)
			m_iState ^= STATE_IDLE;

		m_iState |= STATE_WALK;
	}

	else
	{
		m_iState = STATE_RESET;
		m_iState |= STATE_IDLE;
	}

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());


	for (auto& pPartObject : m_Parts)
		pPartObject->Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
	for (auto& pPartObject : m_Parts)
		pPartObject->Late_Update(fTimeDelta);

	if (GetKeyState(VK_LBUTTON) & 0x8000)
	{
		_float3		vPickPos;
		if (true == m_pGameInstance->Picking(&vPickPos))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPickPos), 1.f));		
	}

	/*static _float fTimeAcc = { 0.f };

	fTimeAcc += fTimeDelta;

	if (fTimeAcc >= 0.05f)
	{*/
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vFixPos = m_pGameInstance->Compute_Height(vPosition, XMMatrixLookAtLH(XMVectorSet(64.f, 30.f, 64.0f, 1.f), XMVectorSet(64.f, 0.f, 64.0f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 0.f)),
			XMMatrixOrthographicLH(200.f, 200.f, 0.f, 50.f));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, XMVectorGetY(vFixPos)));

		/*fTimeAcc = 0.f;
	}*/

	

	m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
#ifdef _DEBUG
	m_pGameInstance->Add_DebugObject(m_pColliderCom);
	m_pGameInstance->Add_DebugObject(m_pNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{
//#ifdef _DEBUG
//	m_pColliderCom->Render();
//	m_pNavigationCom->Render();
//#endif

	return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NaviDesc{};

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CBounding_AABB::BOUNDING_AABB_DESC			ColliderDesc{};
	ColliderDesc.vExtents = _float3(0.5f, 1.0f, 0.5f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
	/* ���� �߰��ϰ� ���� ��Ʈ������Ʈ�� ������ŭ ����� �����س���. */
	m_Parts.resize(PART_END);

	CBody_Player::BODY_DESC		BodyDesc{};
	BodyDesc.pParentState = &m_iState;
	BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	if (FAILED(__super::Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
		return E_FAIL;

	CWeapon::WEAPON_DESC		WeaponDesc{};
	WeaponDesc.pParentState = &m_iState;
	WeaponDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	WeaponDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("SWORD");

	if (FAILED(__super::Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon"), &WeaponDesc)))
		return E_FAIL;

	CParticle_Explosion::EFFECT_DESC		EffectDesc{};
	EffectDesc.pParentState = &m_iState;
	EffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
	EffectDesc.pSocketBoneMatrix = dynamic_cast<CBody_Player*>(m_Parts[PART_BODY])->Get_BoneMatrix_Ptr("SWORD");

	if (FAILED(__super::Add_PartObject(PART_EFFECT, TEXT("Prototype_GameObject_Particle_Explosion"), &EffectDesc)))
		return E_FAIL;

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
}
