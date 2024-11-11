#pragma once
#include "Client_Defines.h"
#include "Level.h"


BEGIN(Engine)
class CTexture;
class CGameObject;
class CUI;
END


BEGIN(Client)



class CLevel_MapTool final : public CLevel
{
public:
	enum UI_LAYERGROUP
	{
		LAYER_UI,
		LAYER_UI_FOR_BATCH,
		LAYER_UIBATCH,
		
		UI_LAYERGROUP_END
	};

public:
	enum class UIATLAS
	{
		ACHIEVEMENTS,
		ADVENTURE,
		ADVENTURE_ACT_01,
		ADVENTURE_ACT_02,
		ADVENTURE_ACT_03,
		ADVENTURE_MAIN,
		ALL,
		ALLOUTWAR,
		ATTRIBUTE,
		BG,
		BLESSINGTEMPLE,
		CASHSHOP,
		CHAT,
		COMMON,
		COMMON_FULLSCREEN,
		CONSTRUCT,
		CONSUMEBAG,
		COOK,
		COSTUME,
		CUSTOMIZE,
		CUSTOMIZE_PRESET,
		DEAD,
		DELIVERY,
		DMG,
		DUNGEON,
		EMOJI,
		EQUIP,
		EVENT,
		FACTION,
		FACTIONDUNGEON,
		FACTIONELECTION,
		FACTIONMANDATE,
		FACTIONSPEAKER,
		FIELDBOSS,
		FISH,
		FORGE,
		FRONTIERWAR,
		GUILD,
		GUILD_TERRITORY_AGO,
		GUILD_TERRITORY_ARTHDAL,
		GUILD_TERRITORY_LAWLESS,
		GUILDCREST_LARGE,
		GUILDCREST_SMALL,
		HISTORYBOOK,
		HUD,
		HUD_ALLOUTWAR,
		HUD_FACTIONWAR,
		ITEMBG,
		ITEMRESTORE,
		LIFESKILL,
		LOADING,
		LOBBY,
		LOGIN,
		MENU,
		MERCENARYQUEST,
		MINIMAP,
		MONSTER,
		OPTION,
		PARTY,
		PKMENU,
		PORTRAIT,
		PUZZLE,
		QTE,
		QUEST,
		RESYNTHESLS,
		RIDE,
		SHORTCUT,
		SKILL,
		SPIRIT,
		WEAPON,
		WORLDMAP,
		ZONESPAWN,

		ATLAS_END
	};
	
	enum class UIICON
	{
		ACC,
		ARMOR,
		BM,
		COSTUME,
		CURRENCY,
		ITEM,
		ITEM_COLLECTIONOBJ,
		ITEM_FISH,
		ITEM_SKILLBOOK,
		RIDE,
		SKILL,
		SPIRIT,
		WEAPON,

		ICON_END
	};

	enum class TEXTURE
	{
		ADVENTURE,
		ALLOUTWAR,
		BANNER,
		BG,
		BLESSINGTEMPLE,
		CIBI,
		COSTUME,
		CUTSCENE,
		DEV,
		DEVICESTATUS,
		EMOJI,
		ETC,
		EVENT,
		FACTION,
		GUIDE,
		GUILD,
		HISTORYBOOK,
		ICONBG,
		INTEGRATIONFIELD,
		LARGEDECO,
		LOADING,
		MAP,
		MERCENARYQUEST,
		PORTRAIT,
		RIDING,
		SERVERTRANSFER,
		SHOP,
		SPIRIT,
		TILING,
		TIMEDUNGEON,
		TUTORIAL,
		
		TEXTURE_END
	};

	typedef struct
	{
		CTexture*		pTexture = { nullptr };
		_wstring		LstrPrototypeName = L"";

	}TEXTURE_INFO;



private:
	CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_MapTool() = default;


public:
	static void	Set_PickedUI(CUI* pPickedUI)
	{
		m_pPickedUI = pPickedUI;
	}

	static CUI* Get_PickedUI() { return m_pPickedUI; }



public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	_float3				m_MousePos = {};
	static CUI*			m_pPickedUI;			// 메인으로 내가 픽할 UI

	CUI*				m_pChildUI = {nullptr};			// 자식으로 선택할 UI
	CUI*				m_pParentUI = {nullptr};			// 부모로 선택할 UI
	class CFreeCamera*  m_pFreeCamera = { nullptr };
	
private:
	vector<TEXTURE_INFO>	m_TextureUIAtlas[(_uint)UIATLAS::ATLAS_END];
	vector<TEXTURE_INFO>	m_TextureUIIcon[(_uint)UIICON::ICON_END];
	vector<TEXTURE_INFO>	m_TextureUITexture[(_uint)TEXTURE::TEXTURE_END];
	vector<TEXTURE_INFO>	m_TextureUIMaterial;

private:
	_bool				m_isShowLoadTexture = false;
	_uint				m_iTextureSizeX = 500;				// Imgui::Image에 표시될 사이즈의 크기지 실제 텍스쳐의 크기가아님
	_uint				m_iTextureSizeY = 500;				// Imgui::Image에 표시될 사이즈의 크기지 실제 텍스쳐의 크기가아님

private:
	_bool				m_isActiveDrag = false;				// 드래그 할래?
	_float2				m_vStartUV = {};
	_float2				m_vEndUV = {};
	

private:
	_bool				m_isActiveChangeTexture = false;	// 텍스쳐 바꿀래?
	_wstring			m_LstrChangeTexturePrototypeName = L"";


private:
	HRESULT Ready_Camera();
	HRESULT	Ready_ImGui();

	void    ImGui_Render();					// 기본 ImGui
	void    Second_ImGui_Render();			// 현재 레벨에 있는 오브젝트의 갯수표현
	void	Third_ImGui_Render();			// 애니메이션에 관련된 정보들을 표현



private:
	void	 SelectTextureFile();
	void	 SelectTextureFolder();
	void	 LoadAllFilesInFolder(const _wstring& FolderPath);

	_uint	 GetFileCountFolder(const _wstring& FolderPath);

	void	 ShowTextures();


private:
	ID3D11ShaderResourceView* CreateCroppedTextureSRV(ID3D11ShaderResourceView* pOriginSRV, _uint iLeft, _uint iTop, _uint iRight, _uint iBottom);


private:
	_wstring GetAtlasName(UIATLAS atlas);
	_wstring GetIconName(UIICON Icon);
	_wstring GetTextureName(TEXTURE Texture);

	_bool ContainsExactWord(const wstring& filePath, const wstring& word);


private:
	void	Save_DefalutUI();
	void	Save_DefalutUIWithChild(CUI* pUI, FILE* fout);
	void	Save_BatchUI();

	
	void	Save_SkillMenuUI();
	void    Load_SkillMenuUI();

	
	void    Load_DefalutUI();
	CUI*    Load_DefalutUIWithChild(FILE* fin);
	void    Load_BatchUI();






	void	 ReadString(FILE* file, _wstring& str);

	_wstring  ConvertToBinRelativePath(const _wstring& FullPath);				// 풀패스에따른 상대경로 추출
	_wstring  ConvertPathToTexturePrototypeName(const _wstring& FullPath);		// 풀패스에 따라 규칙을 통한 텍스쳐 프로토타입이름 추출


public:
	static CLevel_MapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END