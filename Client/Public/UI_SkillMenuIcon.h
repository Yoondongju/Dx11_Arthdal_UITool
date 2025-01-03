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

class CUI_SkillMenuIcon final : public CUI
{
public:
    enum ICON_TYPE 
    {
        JOB_ARCHER_SELECT1 = 5,       // ���� �ü� ���� 1
        JOB_ARCHER_SELECT2 = 6,       // ���� �ü� ���� 2
        JOB_WARRIOR_SELECT1 = 2,      // ���� ���� ���� 1
        JOB_WARRIOR_SELECT2 = 7,      // ���� ���� ���� 2

        ALPHA_DECREASE1 = 8,          // ���İ� ������� 1
        ALPHA_DECREASE2 = 9,          // ���İ� ������� 2

        PLUS_SIGN = 11,               // +��ȣ
        EXIT_BUTTON = 12,             // ������ ��ư


        // ���� - ��Ƽ��
        SKILL_SWORD_THRUST = 39,            // Į ���
        SKILL_SWORD_SWING = 40,             // Į �ֵθ���
        SKILL_SWORD_WAVE_SLASH = 62,        // Į �ĵ������ ���
        SKILL_FIST_GROUND_SLAM = 42,        // �ָ����� �ٴ����
        SKILL_SHIELD_CHARGE = 43,           // ���з� ����
        SKILL_SHIELD_THROW = 44,            // ���� ������
        SKILL_SHIELD_STRENGTHEN = 45,       // ���а�ȭ
        SKILL_SWORD_GROUND_STAB = 46,       // Į�� �����

        // ���� - �нú�
        SKILL_PARRY = 41,                   // �и�
        SKILL_HEALTH_BOOST =64,            // ü�°�ȭ
        SKILL_PHYSICAL_STRENGTH_BOOST = 65, // ��ü�ɷ°�ȭ
        SKILL_PHYSICAL_DEFENSE_BOOST = 67,  // �������� ����
        SKILL_MAGIC_DEFENSE_BOOST = 66,     // �������� ����


        // ���� - ��Ƽ��
        SKILL_AXE_SWING = 47,               // ���� �ֵθ���
        SKILL_AXE_SMASH = 48,               // ���� ���
        SKILL_ADRENALINE = 49,              // �Ƶ巹����
        SKILL_JUDGMENT = 50,                // ������Ʈ
        SKILL_AIR_SMASH = 51,               // ���߿��� ���
        SKILL_SHOUTING = 52,                // ������
        SKILL_AXE_QUICK_SWING = 54,         // ������ ������ �ֵθ���
        SKILL_FIST_X_GROUND_SLAM = 38,      // �ָ����� �ٴ� X�� ���� ���

        // ���� - �нú�
        SKILL_SENSOR_ENHANCEMENT = 71,      // ������ȭ
        SKILL_STATUS_ENHANCEMENT = 69,      // �����̻�ȭ
        SKILL_SHOUT_RANGE_INCREASE = 70,    // ������ ���� ����
        SKILL_DEBUFF_VISIBLE_ENEMIES = 63,  // �þ߿� ���̴� �� �ɷ�ġ �϶�
        SKILL_BASE_MOVE_SPEED_INCREASE = 72,// �⺻ �̵� �ӵ� ����

        // Ȱ - ��Ƽ��
        SKILL_PENETRATING_ARROW = 55,       // ����ȭ��
        SKILL_RAPID_ARROW = 56,             // ���ǵ�ȭ��
        SKILL_BACKSHOT = 57,                // �鼦
        SKILL_FLAME_ARROW = 60,             // ��ȭ��
        SKILL_ARROW_RAIN = 59,              // ȭ���
        SKILL_BACKFLIP_SHOOT = 58,          // �ڷ� ���ϸ鼭 ���߿� �����鼭 Ȱ���
        SKILL_SNIPING = 61,                 // ��������
        SKILL_HAWK_EYE = 53,                // ȣũ����

        // Ȱ - �нú�
        SKILL_AGILITY_ENHANCEMENT = 76,          // ��ø�� ��ȭ
        SKILL_POISON_ARROW = 74,                 // ȭ�쿡 �� �߰�
        SKILL_AFTERIMAGE = 75,                   // �ܻ� �����
        SKILL_BACKSHOT_DISTANCE_INCREASE = 68,   // �鼦�ϴ� �Ÿ� ����
        SKILL_ARROW_DAMAGE_INCREASE = 73         // ȭ�� ���ݷ� ����
    };

private:
	CUI_SkillMenuIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_SkillMenuIcon(const CUI_SkillMenuIcon& Prototype);
	virtual ~CUI_SkillMenuIcon() = default;

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
	static CUI_SkillMenuIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END