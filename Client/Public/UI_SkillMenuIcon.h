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
        SKILL_SWORD_THRUST = 26,            // Į ���
        SKILL_SWORD_SWING = 27,             // Į �ֵθ���
        SKILL_SWORD_WAVE_SLASH = 49,        // Į �ĵ������ ���
        SKILL_FIST_GROUND_SLAM = 29,        // �ָ����� �ٴ����
        SKILL_SHIELD_CHARGE = 30,           // ���з� ����
        SKILL_SHIELD_THROW = 31,            // ���� ������
        SKILL_SHIELD_STRENGTHEN = 32,       // ���а�ȭ
        SKILL_SWORD_GROUND_STAB = 33,       // Į�� �����

        // ���� - �нú�
        SKILL_PARRY = 28,                   // �и�
        SKILL_HEALTH_BOOST = 51,            // ü�°�ȭ
        SKILL_PHYSICAL_STRENGTH_BOOST = 52, // ��ü�ɷ°�ȭ
        SKILL_PHYSICAL_DEFENSE_BOOST = 53,  // �������� ����
        SKILL_MAGIC_DEFENSE_BOOST = 54,     // �������� ����


        // ���� - ��Ƽ��
        SKILL_AXE_SWING = 34,               // ���� �ֵθ���
        SKILL_AXE_SMASH = 35,               // ���� ���
        SKILL_ADRENALINE = 36,              // �Ƶ巹����
        SKILL_JUDGMENT = 37,                // ������Ʈ
        SKILL_AIR_SMASH = 38,               // ���߿��� ���
        SKILL_SHOUTING = 39,                // ������
        SKILL_AXE_QUICK_SWING = 41,         // ������ ������ �ֵθ���
        SKILL_FIST_X_GROUND_SLAM = 25,      // �ָ����� �ٴ� X�� ���� ���

        // ���� - �нú�
        SKILL_SENSOR_ENHANCEMENT = 58,      // ������ȭ
        SKILL_STATUS_ENHANCEMENT = 56,      // �����̻�ȭ
        SKILL_SHOUT_RANGE_INCREASE = 57,    // ������ ���� ����
        SKILL_DEBUFF_VISIBLE_ENEMIES = 50,  // �þ߿� ���̴� �� �ɷ�ġ �϶�
        SKILL_BASE_MOVE_SPEED_INCREASE = 59,// �⺻ �̵� �ӵ� ����

        // Ȱ - ��Ƽ��
        SKILL_PENETRATING_ARROW = 42,       // ����ȭ��
        SKILL_RAPID_ARROW = 43,             // ���ǵ�ȭ��
        SKILL_BACKSHOT = 44,                // �鼦
        SKILL_FLAME_ARROW = 47,             // ��ȭ��
        SKILL_ARROW_RAIN = 46,              // ȭ���
        SKILL_BACKFLIP_SHOOT = 45,          // �ڷ� ���ϸ鼭 ���߿� �����鼭 Ȱ���
        SKILL_SNIPING = 48,                 // ��������
        SKILL_HAWK_EYE = 40,                // ȣũ����

        // Ȱ - �нú�
        SKILL_AGILITY_ENHANCEMENT = 63,          // ��ø�� ��ȭ
        SKILL_POISON_ARROW = 61,                 // ȭ�쿡 �� �߰�
        SKILL_AFTERIMAGE = 62,                   // �ܻ� �����
        SKILL_BACKSHOT_DISTANCE_INCREASE = 55,   // �鼦�ϴ� �Ÿ� ����
        SKILL_ARROW_DAMAGE_INCREASE = 60         // ȭ�� ���ݷ� ����
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