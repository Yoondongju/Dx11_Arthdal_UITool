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
        JOB_ARCHER_SELECT1 = 5,       // 직업 궁수 선택 1
        JOB_ARCHER_SELECT2 = 6,       // 직업 궁수 선택 2
        JOB_WARRIOR_SELECT1 = 2,      // 직업 전사 선택 1
        JOB_WARRIOR_SELECT2 = 7,      // 직업 전사 선택 2

        ALPHA_DECREASE1 = 8,          // 알파값 낮춰야함 1
        ALPHA_DECREASE2 = 9,          // 알파값 낮춰야함 2

        PLUS_SIGN = 11,               // +기호
        EXIT_BUTTON = 12,             // 나가기 버튼


        // 전사 - 액티브
        SKILL_SWORD_THRUST = 26,            // 칼 찌르기
        SKILL_SWORD_SWING = 27,             // 칼 휘두르기
        SKILL_SWORD_WAVE_SLASH = 49,        // 칼 파동생기며 배기
        SKILL_FIST_GROUND_SLAM = 29,        // 주먹으로 바닥찍기
        SKILL_SHIELD_CHARGE = 30,           // 방패로 돌진
        SKILL_SHIELD_THROW = 31,            // 방패 날리기
        SKILL_SHIELD_STRENGTHEN = 32,       // 방패강화
        SKILL_SWORD_GROUND_STAB = 33,       // 칼로 땅찍기

        // 전사 - 패시브
        SKILL_PARRY = 28,                   // 패링
        SKILL_HEALTH_BOOST = 51,            // 체력강화
        SKILL_PHYSICAL_STRENGTH_BOOST = 52, // 신체능력강화
        SKILL_PHYSICAL_DEFENSE_BOOST = 53,  // 물리방어력 증가
        SKILL_MAGIC_DEFENSE_BOOST = 54,     // 마법방어력 증가


        // 도끼 - 액티브
        SKILL_AXE_SWING = 34,               // 도끼 휘두르기
        SKILL_AXE_SMASH = 35,               // 도끼 찍기
        SKILL_ADRENALINE = 36,              // 아드레날린
        SKILL_JUDGMENT = 37,                // 저지먼트
        SKILL_AIR_SMASH = 38,               // 공중에서 찍기
        SKILL_SHOUTING = 39,                // 샤우팅
        SKILL_AXE_QUICK_SWING = 41,         // 도끼로 빠르게 휘두르기
        SKILL_FIST_X_GROUND_SLAM = 25,      // 주먹으로 바닥 X자 남게 찍기

        // 도끼 - 패시브
        SKILL_SENSOR_ENHANCEMENT = 58,      // 센서강화
        SKILL_STATUS_ENHANCEMENT = 56,      // 상태이상강화
        SKILL_SHOUT_RANGE_INCREASE = 57,    // 샤우팅 범위 증가
        SKILL_DEBUFF_VISIBLE_ENEMIES = 50,  // 시야에 보이는 적 능력치 하락
        SKILL_BASE_MOVE_SPEED_INCREASE = 59,// 기본 이동 속도 증가

        // 활 - 액티브
        SKILL_PENETRATING_ARROW = 42,       // 관통화살
        SKILL_RAPID_ARROW = 43,             // 레피드화살
        SKILL_BACKSHOT = 44,                // 백샷
        SKILL_FLAME_ARROW = 47,             // 불화살
        SKILL_ARROW_RAIN = 46,              // 화살비
        SKILL_BACKFLIP_SHOOT = 45,          // 뒤로 백하면서 공중에 오르면서 활쏘기
        SKILL_SNIPING = 48,                 // 스나이핑
        SKILL_HAWK_EYE = 40,                // 호크아이

        // 활 - 패시브
        SKILL_AGILITY_ENHANCEMENT = 63,          // 민첩함 강화
        SKILL_POISON_ARROW = 61,                 // 화살에 독 추가
        SKILL_AFTERIMAGE = 62,                   // 잔상 남기기
        SKILL_BACKSHOT_DISTANCE_INCREASE = 55,   // 백샷하는 거리 증가
        SKILL_ARROW_DAMAGE_INCREASE = 60         // 화살 공격력 증가
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