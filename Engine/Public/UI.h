#pragma once
#include "GameObject.h"

BEGIN(Engine)

class CTransform;

class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct
	{
		_wstring	strFontText = L"";
		_float4		vFontColor = {};
		

	} FONT_INFO;


	typedef struct : public CGameObject::GAMEOBJECT_DESC
	{
		_float	fX{}, fY{}, fSizeX{}, fSizeY{};	
		_float2 vStartUV = {0.f,0.f};
		_float2 vEndUV = {1.f,1.f};
		_float2 vTrigerTargetStartUV = { 0.f,0.f };
		_float2 vTrigerTargetEndUV = { 1.f,1.f };
		
		_bool	 isLoadFromDesc = false;
		_wstring LstrTexturePath = L"";
		_wstring LstrTexturePrototype = L"";

		FONT_INFO FontInfo = {};

	} UI_DESC;

	



	enum UI_EVENT_TYPE
	{
		MOUSE_ON,					// 이번프레임에 마우스 위에 올렷니
		MOUSE_OUT,					// 이번프레임에 마우스 위에 나갓니
		MOUSE_LBTN_TAP,				// 이번프레임에 마우스로 딱 누른시점 (안 뗀 상태)
		MOUSE_LBTN_CLICK,			// 이번프레임에 마우스로 딱 클릭한 시점  

		MOUSE_END
	};

	enum UI_TYPE
	{
		DEFAULT,
		BATCHING,

		UI_TYPE_END
	};



protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CGameObject& Prototype);
	virtual ~CUI() = default;

public:
	void SetMovement(bool _b) { m_bMovement = _b; }
	bool GetMovement() { return m_bMovement; }

	bool IsMouseOn() { return m_bMouseOn; }
	bool IsLbtnDown() { return m_bLbtnDown; }

	void SetParent(CUI* pParentUI) 
	{ 
		m_pParentUI = pParentUI;

		if (nullptr == pParentUI)
			return;


		//m_LstrLayerName = L"NoLayer_ToChild";

		_float fParentX = pParentUI->Get_fX();
		_float fParentY = pParentUI->Get_fY();
		_float fParentSizeX = pParentUI->Get_fSizeX();
		_float fParentSizeY = pParentUI->Get_fSizeY();


		m_fOffsetRatioX = (m_fX - fParentX) / fParentSizeX;
		m_fOffsetRatioY = (m_fY - fParentY) / fParentSizeY;
		m_fOffsetSizeRatioX = m_fSizeX / fParentSizeX;
		m_fOffsetSizeRatioY = m_fSizeY / fParentSizeY;
	}




	CUI* GetParent() { return m_pParentUI; }
	list<CUI*>& GetChildUI() { 	return m_childUI_List; }

public:
	void SetActivate(bool _b)
	{
		m_bActivate = _b;

		if (!m_bActivate)
		{
			m_bMovement = false;
		}
	}
	bool IsActivate() { return m_bActivate; }

	void	Active_ChangeUV(_float2 vChangeStartUV , _float2 vChangeEndUV) 
	{ 
		m_vCurStartUV = vChangeStartUV;
		m_vCurEndUV = vChangeEndUV;
	}
	void	UnActive_ChangeUV()
	{
		m_vCurStartUV = {0.f,0.f};
		m_vCurEndUV = { 1.f,1.f };
	}
	_float2		Get_StartUV() { return m_vCurStartUV; }
	_float2		Get_EndUV() { return m_vCurEndUV; }


	void		Set_TrigerTargetStartUV(_float2 vStartUV) { m_vTrigerTargetStartUV = vStartUV; }
	void		Set_TrigerTargetEndUV(_float2 vEndUV) { m_vTrigerTargetEndUV = vEndUV; }
	_float2		Get_TrigerTargetStartUV() { return m_vTrigerTargetStartUV; }
	_float2		Get_TrigerTargetEndUV() { return m_vTrigerTargetEndUV; }

	void		Set_TrigerUV(_bool b) { m_isActiveTrigerUV = b; }
	_bool		Is_ActiveTrigerUV() { return m_isActiveTrigerUV; }


	void		Set_CompleteBatch() { m_isCompleteBatch = true; }
	UI_TYPE		Get_UIType() { return m_eUIType; }

public:
	void	Set_fX(_float fX) { m_fX = fX; }
	void	Set_fY(_float fY) { m_fY = fY; }
	void	Set_fSizeX(_float fSizeX) { m_fSizeX = fSizeX; }
	void	Set_fSizeY(_float fSizeY) { m_fSizeY = fSizeY; }

	_float	Get_fX() { return m_fX; }
	_float	Get_fY() { return m_fY; }
	_float	Get_fSizeX() { return m_fSizeX; }
	_float	Get_fSizeY() { return m_fSizeY; }

public:
	_float	Get_fOffsetRatioX() { return m_fOffsetRatioX; }
	_float	Get_fOffsetRatioY() { return m_fOffsetRatioY; }
	_float	Get_fOffsetSizeRatioX() { return m_fOffsetSizeRatioX; }
	_float	Get_fOffsetSizeRatioY() { return m_fOffsetSizeRatioY; }


	void	Set_CurZOrder(_float fZ) { m_fCurZ = fZ; }
	_float	Get_CurZOrder() { return m_fCurZ; }




public:
	virtual void		ChangeTexture(const _wstring& LstrChangeTexturePrototype) {};
	const _wstring&		Get_MyTexturePrototypeName() { return m_LstrMyReferenceTexture; }

	
	const _wstring&		Get_PrototypeName() { return m_LstrPrototypeName; }
	const _wstring&		Get_LayerName() { return m_LstrLayerName; }
	FONT_INFO&			Get_FontInfo_Ref() { return m_Font; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual	void	Render_Text() {};

protected:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float					m_fCurZ = 0.f;

	_float					m_fOffsetRatioX = {0.f};
	_float					m_fOffsetRatioY = {0.f};
	_float					m_fOffsetSizeRatioX = { 0.f };
	_float					m_fOffsetSizeRatioY = { 0.f };



	_float4x4				m_ViewMatrix, m_ProjMatrix;

	_uint					m_iDepth = {};
	_float					m_fViewWidth{}, m_fViewHeight{};


	list<CUI*>				m_childUI_List;

	CUI*					m_pParentUI = { nullptr };	 // 부모 UI가 있니

	_bool					m_bMovement = {};     // UI가 움직일수 있는지?

	_bool					m_bPreMouseOn = {};	  // 이전 프레임에 UI위에 마우스가 올려져 있었는지 ?
	_bool					m_bMouseOn = {};	  // UI 위에 마우스가 있는지


	_bool					m_bLbtnDown = {};	  // UI에 왼쪽버튼이 눌러져 있는지

	_bool					m_bActivate = {true};	 // 활성화
	_bool					m_isUIEventType[UI_EVENT_TYPE::MOUSE_END] = { false };

protected:
	_wstring				m_LstrMyReferenceTexture = L"";				// 텍스쳐 프로토타입이름
	
	_float2					m_vCurStartUV = { 0.f,0.f };
	_float2					m_vCurEndUV = { 1.f,1.f };

	_bool					m_isActiveTrigerUV = { false };
	_float2					m_vTrigerTargetStartUV = { 0.f,0.f };		// 내가 어떤 트리거에 의해 변경될 UV의 좌표
	_float2					m_vTrigerTargetEndUV = { 1.f,1.f };			// 내가 어떤 트리거에 의해 변경될 UV의 좌표


	_bool					m_isCompleteBatch = { false };
	UI_TYPE					m_eUIType = { UI_TYPE::DEFAULT };
	

	FONT_INFO				m_Font = {};			



protected:
	virtual void MouseOnCheck();				// 모든 자식 UI들은 부모꺼를 쓸거임
	virtual void MouseLBtnDown() {};
	virtual void MouseLBtnUp() {};
	virtual void MouseClick() {};
	virtual void MouseOn() {};
	virtual void MouseOut() {};



	virtual void ExcuteMouseLBtnDown(_float fTimeDelta) {};
	virtual void ExcuteMouseClick(_float fTimeDelta) {};
	virtual void ExcuteMouseOn(_float fTimeDelta) {};
	virtual void ExcuteMouseOut(_float fTimeDelta) {};



	void		 SetParentUI(CUI* _p) { m_pParentUI = _p; }




public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;


	friend class CUI_Manager;
};

END
