#include "stdafx.h"
#include "../Public/Level_MapTool.h"

#include "FreeCamera.h"

#include "GameInstance.h"
#include "GameObject.h"
#include "UIBatch.h"


#include <fstream>
#include <iostream>
#include <iomanip>

#include <codecvt>

#include "ImGuiFileDialog.h"



#pragma warning (disable : 4996)

static ImGuizmo::OPERATION eGizmoType = { ImGuizmo::TRANSLATE };
CLevel_MapTool::UI_LAYERGROUP UILayerGroup = CLevel_MapTool::UI_LAYERGROUP::LAYER_UI;
CUIBatch::BATHCING_GROUP UI_BatchingGroup = CUIBatch::BATHCING_GROUP::BATHCING_GROUP_END;

vector<wstring> Prototypeitems;
vector<string> items;
vector<const char*> cstrItems;

CUI* CLevel_MapTool::m_pPickedUI = nullptr;
_bool  g_bFocusImGui;

ImVec2 startDrag(-1, -1); // LT ��ǥ (�巡�� ������)
ImVec2 endDrag(-1, -1);   // RB ��ǥ (�巡�� ������)

ImVec2 startUV(-1, -1); 
ImVec2 endUV(-1, -1);   


_bool isDragging = false;  // �巡�� ���� ����


std::istream& operator>>(std::istream& is, _float4x4& mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            is >> mat.m[i][j];
        }
    }
    return is;
}

std::string wstringToString(const std::wstring& wstr) {
    // Note: std::wstring_convert is deprecated in C++17 and removed in C++20.
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring stringToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::wstring ToLower(const std::wstring& str) {
    std::wstring lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::towlower);
    return lowerStr;
}

std::wstring ToUpper(const std::wstring& str) {
    std::wstring upperStr = str;
    std::transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::towupper);
    return upperStr;
}


CLevel_MapTool::CLevel_MapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel{ pDevice,pContext }
{
}

HRESULT CLevel_MapTool::Initialize()
{
    if (FAILED(Ready_Camera()))
        return E_FAIL;

    if (FAILED(Ready_ImGui()))         // IMGUI
        return E_FAIL;

   
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_KeyPad");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_Menu");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_Icon");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_PlayerHp");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_PlayerMp");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_PlayerPanelHpMp");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_InventoryPanel");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_InventoryStaticIcon");

    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_MainMenuPanel");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_MainMenuIcon");
        
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_SkillMenuDeco");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_SkillMenuIcon");
    Prototypeitems.emplace_back(L"Prototype_GameObject_UI_SkillMenuPanel");
    


    for (const auto& item : Prototypeitems)
    {
        items.push_back(string(item.begin(), item.end()));
        cstrItems.push_back(items.back().c_str());
    }
   

    return S_OK;
}


HRESULT CLevel_MapTool::Ready_ImGui()
{
    // �����κ�
    ImGui::CreateContext();          // Create ������ ���� ������� , �갡 ������ ����������
    ImGui::StyleColorsDark();        // Style -> ����
 
    
    // ImGuizmo Ȱ��ȭ
    ImGuizmo::Enable(true); 

    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig font_cfg;
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 2;
    font_cfg.PixelSnapH = true;

    // ��Ʈ ���� ��θ� �����ϰ� �����ϼ���
    io.Fonts->AddFontFromFileTTF("C://Windows//Fonts//gulim.ttc", 14.0f, nullptr, io.Fonts->GetGlyphRangesKorean());


    if (!ImGui_ImplWin32_Init(g_hWnd))  // win32 ���õ� �ʱ�ȭ
    {
        MSG_BOX(L"IMGUI WIN32 �ʱ�ȭ ����");
        return E_FAIL;
    }

    if (!ImGui_ImplDX11_Init(m_pDevice,m_pContext))  // Dx ���õ� �ʱ�ȭ
    {
        MSG_BOX(L"IMGUI DX11 �ʱ�ȭ ����");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Camera()
{
    CFreeCamera::CAMERA_FREE_DESC		Desc{};

    Desc.fSensor = 0.2f;
    Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
    Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
    Desc.fFovy = XMConvertToRadians(90.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.f;
    Desc.fSpeedPerSec = 30.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

    if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
        return E_FAIL;

    m_pFreeCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Camera());
    Safe_AddRef(m_pFreeCamera);

    return S_OK;
}



void CLevel_MapTool::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Get_KeyState(KEY::P) == KEY_STATE::TAP)
    {
        if (CUIBatch::BATHCING_GROUP::BATHCING_GROUP_END == UI_BatchingGroup)
        {
            MessageBox(NULL, TEXT("��Ī���� ���� �׷��� Ÿ���� ���� �����ϼ���"), TEXT("������"), MB_OK);
            return;
        }

        _int iResult = MessageBox(
            NULL,                              // �θ� â �ڵ� (NULL�� ��� ����)
            TEXT("ȭ�鿡 ��ġ�� �������� UI�� \n�ϳ��� ��ġUI�� �����ǰ���? \n �������� UI���� ��� ������ϴ�.\n�ǵ����� ��������� ����"),    // �޽��� ����
            TEXT("����"),                       // �޽��� ����
            MB_OKCANCEL | MB_ICONQUESTION      // OK�� Cancel ��ư + ���� ������
        );

        if (iResult == IDOK)
        {
            list<CGameObject*>& BatchingLists = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_UIBatch");
            for (auto pObj : BatchingLists)
            {
                if (UI_BatchingGroup == static_cast<CUIBatch*>(pObj)->Get_MyBatchingGroup()) // ���� ��ġ�� �׷��� �ϳ� �̻��ִٶ�� 
                {
                    MessageBox(NULL, TEXT("�ش� ��Ī�׷��� �̹� �����մϴ� �ٸ���ġ�׷��� �����ϼž��մϴ�"), TEXT("������"), MB_OK);
                    return;
                }
            }

            CUIBatch::BATCH_GROUP_DESC Desc;
            Desc.eGroup = UI_BatchingGroup;
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_UIBatch",       // �ӽ�
                TEXT("Prototype_GameObject_UIBatch"), &Desc)))
                MSG_BOX(L"�׽�Ʈ������ �ȳ���");

            MessageBox(NULL, TEXT("���������� ��Ī�߽��ϴ�"), TEXT("������"), MB_OK);
        }
        else if (iResult == IDCANCEL)
        {
            MessageBox(NULL, TEXT("���߿� �ٽ� �õ��ϼ���"), TEXT("������"), MB_OK);
            return;
        }
    }


    if (nullptr != m_pPickedUI && m_pGameInstance->Get_KeyState(KEY::CTRL) == KEY_STATE::HOLD)
    {
        if (m_pGameInstance->Get_KeyState(KEY::C) == KEY_STATE::TAP)
        {
            _wstring LstrLayerName = m_pPickedUI->Get_LayerName();
            _wstring LstrPrototypeName = m_pPickedUI->Get_PrototypeName();

            CUI* pOriginUI = static_cast<CUI*>(m_pPickedUI);

            CUI::UI_DESC		Desc{};
            Desc.fX = pOriginUI->Get_fX();
            Desc.fY = pOriginUI->Get_fY();
            Desc.fSizeX = pOriginUI->Get_fSizeX();
            Desc.fSizeY = pOriginUI->Get_fSizeY();
            Desc.LstrTexturePrototype = pOriginUI->Get_MyTexturePrototypeName();
            Desc.vStartUV = pOriginUI->Get_StartUV();
            Desc.vEndUV = pOriginUI->Get_EndUV();
            Desc.vTrigerTargetStartUV = pOriginUI->Get_TrigerTargetStartUV();
            Desc.vTrigerTargetEndUV = pOriginUI->Get_TrigerTargetEndUV();

            Desc.fSpeedPerSec = 10.f;
            Desc.fRotationPerSec = XMConvertToRadians(90.0f);

            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, LstrLayerName,       // �ӽ�
                LstrPrototypeName , &Desc)))
                MSG_BOX(L"�׽�Ʈ������ �ȳ���");
        }

        if (m_pGameInstance->Get_KeyState(KEY::X) == KEY_STATE::TAP)
        {
            m_pGameInstance->Delete(LEVEL_MAPTOOL, CRenderer::RENDERGROUP::RG_END, m_pPickedUI);

            if (m_pPickedUI == m_pParentUI)
                m_pParentUI = nullptr;
            if (m_pPickedUI == m_pChildUI)
                m_pChildUI = nullptr;
                
            m_pPickedUI = nullptr;
        }
    }
    


    //if (m_pGameInstance->Get_KeyState(KEY::I) == KEY_STATE::TAP)
    //{
    //    list<CGameObject*>& BatchingLists = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_UIBatch");
    //    m_pGameInstance->Delete(LEVEL_MAPTOOL, CRenderer::RG_UI, BatchingLists.front());
    //}
  
}


HRESULT CLevel_MapTool::Render()
{
    SetWindowText(g_hWnd, TEXT("����_�����Դϴ�."));

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();



    
    ImGui_Render();
    Second_ImGui_Render();


   
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


    return S_OK;
}


void CLevel_MapTool::ImGui_Render()
{  
    if(false == m_isActiveDrag)     // �巡���ϰų� �ؽ��� �ٲܶ� ��������
        ImGui::Begin("First_ImGui");
    else
        ImGui::Begin("First_ImGui" , nullptr , ImGuiWindowFlags_NoMove);


    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))      // ImGui â�� ������ ���� ��������
    {
        g_bFocusImGui = true;
    }
    else
        g_bFocusImGui = false;

    ImGuizmo::SetOrthographic(true);
    ImGuizmo::SetRect(0, 0, g_iWinSizeX, g_iWinSizeY); // ����Ʈ ũ�� ����
    ImGuizmo::BeginFrame();
    

   
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::TextColored(ImVec4(1, 1, 0, 1), u8"�׽�Ʈ");

   
    //SelectTextureFile();

    SelectTextureFolder();
    
    ShowTextures();
    






  
    ImGui::Dummy(ImVec2(0.0f, 10.0f)); // ���̰� 10�� �� ���� �߰�
    if (ImGui::RadioButton("GuizmoMode: Translate", eGizmoType == ImGuizmo::TRANSLATE) ||
        m_pGameInstance->Get_KeyState(KEY::W) == KEY_STATE::TAP)
        eGizmoType = ImGuizmo::TRANSLATE;
    if (ImGui::RadioButton("GuizmoMode: Rotate", eGizmoType == ImGuizmo::ROTATE) ||
        m_pGameInstance->Get_KeyState(KEY::E) == KEY_STATE::TAP)
        eGizmoType = ImGuizmo::ROTATE;
    if (ImGui::RadioButton("GuizmoMode: Scale", eGizmoType == ImGuizmo::SCALE) ||
        m_pGameInstance->Get_KeyState(KEY::R) == KEY_STATE::TAP)
        eGizmoType = ImGuizmo::SCALE;


    static wstring PrototypeName = L"";
    if (ImGui::TreeNode(u8"UI��ü�� ������ PrototypeName ����"))
    {
        static _int iCurrentItemIndex = 0;
        if (ImGui::ListBox("UI ��Ī �׷�", &iCurrentItemIndex, cstrItems.data(), cstrItems.size()))
        {
            PrototypeName = Prototypeitems[iCurrentItemIndex];
        }
        ImGui::TreePop();
    }

    string Name = "���� PrototypeName:  " + wstringToString(PrototypeName);
    ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), Name.c_str());
    if (ImGui::Button(u8"Defalut�� UI����"))
    {
        if (L"" == PrototypeName)
            MSG_BOX(L"PrototypeName�� ���� �����ϼ���.");
        else
        {
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_UI",   // �ӽ�   L"Layer_UI_ForBatch"
                PrototypeName)))
                MSG_BOX(L"��׶��� ������ �ȳ���");
        }   
    }
    if (ImGui::Button(u8"Batch�� UI����"))
    {
        if (L"" == PrototypeName)
            MSG_BOX(L"PrototypeName�� ���� �����ϼ���.");
        else
        {
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_UI_ForBatch",   // �ӽ�   L"Layer_UI_ForBatch"
                PrototypeName)))
                MSG_BOX(L"��׶��� ������ �ȳ���");
        }   
    }




    if (ImGui::Button(u8"Defalut UI����"))
        Save_DefalutUI();
    if (ImGui::Button(u8"Batch UI����"))
        Save_BatchUI();
    
    if (ImGui::Button(u8"Defalut UI�ε�"))
        Load_DefalutUI();
    if (ImGui::Button(u8"Batch UI�ε�"))
        Load_BatchUI();


    if (ImGui::Button(u8"SkillMenu UI����"))
        Save_SkillMenuUI();
    if (ImGui::Button(u8"SkillMenu UI�ε�"))
        Load_SkillMenuUI();






    ImGui::EndChild();
    ImGui::End();
    



    //   =============== ImGuizmo =============== 
    if (nullptr != m_pPickedUI)  // ī�޶� ���� ���� ������Ʈ ��ġ�� ���������� nullptr�� ��ȯ
    {     
        static CUI* pPrePickedObj = nullptr;
        static _bool     bFristSetting = false;
        static _float4x4 WorldMatrix = {};
        static _float4x4 ViewMatrix = {};
        static _float4x4 ProjMatrix = {};


        if (!bFristSetting || pPrePickedObj != m_pPickedUI)
        {
            XMStoreFloat4x4(&WorldMatrix, m_pPickedUI->Get_Transform()->Get_WorldMatrix());

            pPrePickedObj = m_pPickedUI;
            bFristSetting = true;
        }               
        
        XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
        XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(1600.f, 900.f, 0.f, 1.f));

        if (ImGuizmo::Manipulate((_float*)(&ViewMatrix), (_float*)(&ProjMatrix), eGizmoType, ImGuizmo::LOCAL, (_float*)(&WorldMatrix)))
        {                  
            _vector worldPosition = XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2], 1.0f);
            _matrix viewProjMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ViewMatrix), XMLoadFloat4x4(&ProjMatrix));
            _vector screenPosition = XMVector3TransformCoord(worldPosition, viewProjMatrix);
            
            _float fScreenX = XMVectorGetX(screenPosition) * 1600.f * 0.5f + 1600.f * 0.5f;
            _float fScreenY = -XMVectorGetY(screenPosition) * 900.f * 0.5f + 900.f * 0.5f;
     
            m_pPickedUI->Get_Transform()->Set_WorldMatrix_Ptr(&WorldMatrix);


            m_pPickedUI->Set_fX(fScreenX);
            m_pPickedUI->Set_fY(fScreenY);
          

            _vector vRight = XMLoadFloat3(&_float3(WorldMatrix.m[0][0], WorldMatrix.m[0][1], WorldMatrix.m[0][2]));
            _vector vUp = XMLoadFloat3(&_float3(WorldMatrix.m[1][0], WorldMatrix.m[1][1], WorldMatrix.m[1][2]));

            _float m_fSizeX = XMVectorGetX(XMVector3Length(vRight));       //  ��Ʈ x^2 + y^2 + z^2 �� �����Ǳ��̴ϱ� 
            _float m_fSizeY = XMVectorGetX(XMVector3Length(vUp));
          
            m_pPickedUI->Set_fSizeX(m_fSizeX);
            m_pPickedUI->Set_fSizeY(m_fSizeY);
        }        
    }
}

void CLevel_MapTool::Second_ImGui_Render()
{
    //ImGui::SetNextWindowSize(ImVec2(200, 300));
    ImGui::Begin("Second_ImGui");

    string strLeft = "MousePos_Left:" + to_string(startDrag.x);
    string strTop = "MousePos_Top:" + to_string(startDrag.y);
    string strRight = "MousePos_Right:" + to_string(endDrag.x);
    string strBottom = "Mouse Pos_Bottom:" + to_string(endDrag.y);
    ImGui::Text(strLeft.c_str());
    ImGui::Text(strTop.c_str());
    ImGui::Text(strRight.c_str());
    ImGui::Text(strBottom.c_str());


    //string strUVLeft = "UV_Left:" + to_string(startUV.x);
    //string strUVTop = "UV_Top:" + to_string(startUV.y);
    //string strUVRight = "UV_Right:" + to_string(endUV.x);
    //string strUVBottom = "UV_Bottom:" + to_string(endUV.y);
    //ImGui::Text(strUVLeft.c_str());
    //ImGui::Text(strUVTop.c_str());
    //ImGui::Text(strUVRight.c_str());
    //ImGui::Text(strUVBottom.c_str());
    //
    //
    //// ������ UV 
    //string strExtractUVLeft = "ExtractUV_Left:" + to_string(m_vStartUV.x);
    //string strExtractUVTop = "ExtractUV_Top:" + to_string(m_vStartUV.y);
    //string strExtractUVRight = "ExtractUV_Right:" + to_string(m_vEndUV.x);
    //string strExtractUVBottom = "ExtractUV_Bottom:" + to_string(m_vEndUV.y);
    //ImGui::Text(strExtractUVLeft.c_str());
    //ImGui::Text(strExtractUVTop.c_str());
    //ImGui::Text(strExtractUVRight.c_str());
    //ImGui::Text(strExtractUVBottom.c_str());

    // ������ �ؽ����� ����Ÿ���̸�
    string strExtractTexturePrototypeName = "ExtractTexture_PrototypeName: " + wstringToString(m_LstrChangeTexturePrototypeName);
    ImGui::Text(strExtractTexturePrototypeName.c_str());

    if (ImGui::Button(u8"Texture Drag On/Off"))
    {
        if(false == m_isActiveDrag)
            m_isActiveDrag = true;
        else
        {
            m_isActiveDrag = false;
            m_vStartUV = { 0.f,0.f };
            m_vEndUV = { 1.f,1.f }; 
        }            
    }

    if (ImGui::Button(u8"Texture Change On/Off"))
    {
        if (false == m_isActiveChangeTexture)
            m_isActiveChangeTexture = true;
        else      
            m_isActiveChangeTexture = false;

        m_isActiveDrag = false;
        m_vStartUV = { 0.f,0.f };
        m_vEndUV = { 1.f,1.f };
    }


    if (ImGui::Button(u8"����UV �����ϱ�"))
    {
        if (nullptr == m_pPickedUI)
            MSG_BOX(L"������ UI�� ���� Ŭ���ϼ���");
        else
        {
            m_pPickedUI->Active_ChangeUV(m_vStartUV,m_vEndUV);
        }
    }
    
    if (ImGui::Button(u8"Ʈ����Ÿ��UV �����ϱ�"))
    {
        m_pPickedUI->Set_TrigerTargetStartUV(m_vStartUV);
        m_pPickedUI->Set_TrigerTargetEndUV(m_vEndUV);
    }
    
    if (ImGui::Button(u8"Ʈ����Ÿ��UV �����Ű��"))
    {
        m_pPickedUI->Set_TrigerUV(true);
    }
    
    if (ImGui::Button(u8"����UV �����Ű��"))
    {
        m_pPickedUI->Set_TrigerUV(false);
    }


    if (ImGui::Button(u8"Change Texture"))
    {
        if (nullptr == m_pPickedUI)
            MSG_BOX(L"������ UI�� ���� Ŭ���ϼ���");
        else if (L"" == m_LstrChangeTexturePrototypeName)
            MSG_BOX(L"��ü�� �ؽ��ĸ� ���� �����ϼ���");
        else
        {
            m_pPickedUI->Active_ChangeUV(_float2(0.f,0.f), _float2(1.f, 1.f));
            m_pPickedUI->ChangeTexture(m_LstrChangeTexturePrototypeName);
        }
    }



    if (ImGui::TreeNode(u8"UI ��Ī�׷� ����"))
    {
        if (ImGui::RadioButton(u8"BACKGROUND_DEFAULT �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::BACKGROUND_DEFAULT))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::BACKGROUND_DEFAULT;
        }
        if (ImGui::RadioButton(u8"INVENTORY_ICON �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::INVENTORY_ICON))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::INVENTORY_ICON;
        }
        if (ImGui::RadioButton(u8"MAINMENU_ICON �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::MAINMENU_ICON))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::MAINMENU_ICON;
        }
        if (ImGui::RadioButton(u8"MAINMENU_DECO �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::MAINMENU_DECO))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::MAINMENU_DECO;
        }
        if (ImGui::RadioButton(u8"SKILLMENU_DECO �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::SKILLMENU_DECO))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::SKILLMENU_DECO;
        }
        if (ImGui::RadioButton(u8"SKILLMENU_ICON �׷����� ��ġ", UI_BatchingGroup == CUIBatch::BATHCING_GROUP::SKILLMENU_ICON))
        {
            UI_BatchingGroup = CUIBatch::BATHCING_GROUP::SKILLMENU_ICON;
        }
        
      
        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"���� UI��Ī�׷� ����"))
    {
        vector<string> GroupNames;

        list<CGameObject*>& UIBatchs = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, L"Layer_UIBatch");
        for (auto pUI : UIBatchs)
        {
            CUIBatch* pUIBatch = static_cast<CUIBatch*>(pUI);       
            _wstring LstrGroupName = pUIBatch->Get_MyBatchingGroupName();

            string groupName(LstrGroupName.begin(), LstrGroupName.end());
            GroupNames.push_back(groupName);            
        }
        
        static _int iCurrentItemIndex = 0;

        // ListBox�� ǥ���� �׸� �迭 ���� �� �ʱ�ȭ
        std::vector<const char*> items;
        for (const auto& name : GroupNames)
        {
            items.push_back(name.c_str());
        }

        if (ImGui::ListBox("UI ��Ī �׷�", &iCurrentItemIndex, items.data(), items.size()))
        {
            auto iter = UIBatchs.begin();           // ��ġ�� ���� ���� ��ġ �����ְ� �ڽļ��� ���� ��¥�� ��ġ���ϰ� ��ġ�ھƵ� ���ö��� �Ⱥ���
            advance(iter, iCurrentItemIndex);
            CUIBatch* pSelectedUIBatch = static_cast<CUIBatch*>(*iter);

            m_pPickedUI = pSelectedUIBatch;
        }
          

        ImGui::TreePop();
    }




    if (ImGui::Button(u8"�θ� �����ϱ�"))
    {
        // ��Ī UI�� �θ� �ɼ�����  ( O )  == ��Ī UI�� �ڽ��� �Ѽ�����  ( O )
        // ������ ��ĪUI�� �⺻UI�� �ڽ����� �����ִ�      
        if (nullptr == m_pPickedUI)
            MSG_BOX(L"�θ�� ������ UI�� ���� �n �ϼ���");
        else if (L"Layer_UI_ForBatch" == m_pPickedUI->Get_LayerName() ||
                L"Layer_UIBatch" == m_pPickedUI->Get_LayerName())   // ��Ī�̿Ϸ�� ��ĪUI�� �̹� ��ŷ���Ҽ����� ��� ��Ī�� ���� UI���� ���̾��̸���������
            MSG_BOX(L"��ĪUI�� �θ� �ɼ������ϴ�.");
        else
            m_pParentUI = m_pPickedUI;      
    }
    ImGui::SameLine(110.f);
    if (ImGui::Button(u8"�ڽ� �����ϱ�"))
    {
        if (nullptr == m_pPickedUI)
            MSG_BOX(L"�ڽ����� ������ UI�� ���� �n �ϼ���");
        else if (L"Layer_UI_ForBatch" == m_pPickedUI->Get_LayerName())  
            MSG_BOX(L"��ĪUI�� ��UI Ȥ�� \n��ĪUI�� ��UI ��, Layer_UI_ForBatch�� ��ü��\n �������� �ڽ��� �ɼ������ϴ�.");
        else
            m_pChildUI = m_pPickedUI;       
    }
    ImGui::SameLine(220.f);
    if (ImGui::Button(u8"�ڽ����� �߰��ϱ�"))
    {
        if (nullptr == m_pParentUI)
            MSG_BOX(L"�θ� UI�� ���� �n�Ͻÿ�");

        else if (nullptr == m_pChildUI)
            MSG_BOX(L"�ڽ� UI�� ���� �n�Ͻÿ�");

        else if (m_pParentUI == m_pChildUI)
            MSG_BOX(L"�θ�� �ڽ��� �ּҰ� �����ϴ� �ȵ�");

        else
        {
            _int iResult = MessageBox(
                NULL,
                L"���� �ڽ����� �߰��Ͻðڽ��ϱ�?\n�ڽ�UI�� ��ġ,ũ�⸦ �ùٸ��� �θ�UI�� ��Ī���׳���?\n�ڽ��߰� �ϴ¼��� OffSet����̵Ǽ� �ٲܼ������ϴ�.",
                TEXT("�ڽ� �߰��ϱ��� �������˻�"),                       // �޽��� ����
                MB_OKCANCEL | MB_ICONQUESTION
            );

            if (iResult == IDCANCEL)
            {
                MessageBox(NULL, TEXT("���߿� �ٽ� �õ��ϼ���"), TEXT("������"), MB_OK);         
            }
            else if (iResult == IDOK)
            {
                const _wstring& ChildUILayerName = m_pChildUI->Get_LayerName();
                m_pGameInstance->RemoveObjectToLayer(LEVEL_MAPTOOL, ChildUILayerName, m_pChildUI);

                list<CUI*>& ChildUIs = m_pParentUI->GetChildUI();
                ChildUIs.emplace_back(m_pChildUI);
                m_pChildUI->SetParent(m_pParentUI);     // �θ����� ���ָ� �ȿ��� ���̾��̸��� �ٲٰ� �����°����� ���� �� ������

                if (L"Layer_UIBatch" == m_pChildUI->Get_LayerName())
                {
                    list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = static_cast<CUIBatch*>(m_pChildUI)->Get_UIList();
                    for (auto UIBatchInfo : UIList)
                    {
                        UIBatchInfo.pUI->SetParent(m_pParentUI);
                    }
                }


                MSG_BOX(L"���������� �ڽ��߰��� �߽��ϴ�");

                m_pChildUI = nullptr;
            }      
        }
    }
 

    _wstring ParnetTag = L"����!";
    _wstring ChildTag = L"����!";

    if (nullptr != m_pParentUI)
    {
        ParnetTag = L"����";
    }      
    if (nullptr != m_pChildUI)
    {
        ChildTag = L"����";
    }      
    _wstring LstrPick_ParentAndChildUI = 
        L"���� �n�� �θ�UI: " + ParnetTag + L"\n"
        + L"���� �n�� �ڽ�UI: " + ChildTag;
   

    m_pGameInstance->Render_Text(TEXT("Font_145"),
        LstrPick_ParentAndChildUI.c_str(),
        XMVectorSet(1600.f - 400.f, 900.f - 165.f, 0.f, 1.f),
        XMVectorSet(0.f, 1.f, 0.f, 1.f),
        0.f,
        XMVectorSet(0.f, 0.f, 0.f, 1.f),
        0.5f);



    if (m_pGameInstance->Get_KeyState(KEY::UP) == KEY_STATE::HOLD)
        m_iTextureSizeY += 10;
    if (m_pGameInstance->Get_KeyState(KEY::DOWN) == KEY_STATE::HOLD)
        m_iTextureSizeY -= 10;
    if (m_pGameInstance->Get_KeyState(KEY::LEFT) == KEY_STATE::HOLD)
        m_iTextureSizeX -= 10;
    if (m_pGameInstance->Get_KeyState(KEY::RIGHT) == KEY_STATE::HOLD)
        m_iTextureSizeX += 10;

    ImGui::End();
}



void CLevel_MapTool::Third_ImGui_Render()
{
    ImGui::SetNextWindowSize(ImVec2(600, 150));
    ImGui::Begin("Third_ImGui");

    

    ImGui::End();
}




void CLevel_MapTool::ReadString(FILE* file, _wstring& str)
{
    _uint length = 0;
    fread(&length, sizeof(length), 1, file);

    str.resize(length);
    fread(&str[0], sizeof(_tchar) , length  , file);
}



void CLevel_MapTool::SelectTextureFile()
{
    // ������ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ������Ÿ�Կ��� �����վ�

    _uint iPrototypeNamingCount = 0;

    if (ImGui::Button((u8"UI ��������")))
    {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFolderDlg", u8"���� ����", ".*");     //  ".*" : �������� ���ϵ� ����
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFolderDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk())    // ok��ư ������ ������´�
        {
            string SelectFilePath = ImGuiFileDialog::Instance()->GetFilePathName();               // ���� ���� ���� ������ �̸��̰���?
            _wstring LSelectFilePath = L"";
            _wstring LSelectTexturePrototypeName = L"";

            LSelectFilePath.assign(SelectFilePath.begin(), SelectFilePath.end());  //  string -> wstring


            LSelectFilePath = ConvertToBinRelativePath(LSelectFilePath);           // ����� ����
            LSelectTexturePrototypeName = ConvertPathToTexturePrototypeName(LSelectFilePath);   // ��α�Ģ�� ���� ������Ÿ���̸���ġ


            LSelectTexturePrototypeName += to_wstring(iPrototypeNamingCount++);

            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, LSelectTexturePrototypeName.c_str(),
                CTexture::Create(m_pDevice, m_pContext, LSelectFilePath.c_str(), 1))))
                MSG_BOX(L"�ؽ��� �ε� ����");            
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

void CLevel_MapTool::SelectTextureFolder()
{
    if (ImGui::Button((u8"UI ��������")))
    {
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFolderDlg", u8"���� ����", nullptr);     //  ".*" : �������� ���ϵ� ����
    }
    if (ImGuiFileDialog::Instance()->Display("ChooseFolderDlg")) {
        if (ImGuiFileDialog::Instance()->IsOk())    // ok��ư ������ ������´�
        {
            string SelectFilePath = ImGuiFileDialog::Instance()->GetCurrentPath();               // ���� ���� ���� ������ �̸��̰���?
            _wstring LSelectFilePath = L"";
          
            LSelectFilePath.assign(SelectFilePath.begin(), SelectFilePath.end());  //  string -> wstring

            LoadAllFilesInFolder(LSelectFilePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }

}

void CLevel_MapTool::LoadAllFilesInFolder(const _wstring& FolderPath)
{
    //_uint iFileCount = GetFileCountFolder(FolderPath);             �ʾ���

    _uint iPrototypeNamingCount = 0;
    _wstring LSearchPath = FolderPath + L"\\*";

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(LSearchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        MSG_BOX(L"������ �� �� �����ϴ�.");
        return;
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
            continue;

        std::wstring filePath = FolderPath + L"\\" + findData.cFileName;

        // ������ ��� ��������� ���� ���� Ž��
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            LoadAllFilesInFolder(filePath);  // ��� ȣ��
        }
        else 
        {          
            _wstring LSelectFilePath = ConvertToBinRelativePath(filePath);
            _wstring LSelectTexturePrototypeName = ConvertPathToTexturePrototypeName(filePath);

 
            LSelectTexturePrototypeName += to_wstring(iPrototypeNamingCount++);     // �ؽ����� �����̸��� �����

            CTexture* pOut = nullptr;
            if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, LSelectTexturePrototypeName.c_str(),
                CTexture::Create(m_pDevice, m_pContext, LSelectFilePath.c_str(), 1 , &pOut)))) {
                MSG_BOX(L"�ؽ�ó �ε� ����");
            }
            else
            {
                if (filePath.find(L"Atlas") != std::wstring::npos) 
                {
                    _wstring UpperFilePath = ToUpper(filePath);
                    for (_uint i = 0; i < (_uint)UIATLAS::ATLAS_END; i++)
                    {
                        _wstring AtlassName = GetAtlasName((UIATLAS)i);
                        if (ContainsExactWord(UpperFilePath, AtlassName))
                        {              
                            TEXTURE_INFO Info = {};
                            Info.pTexture = pOut;
                            Info.LstrPrototypeName = LSelectTexturePrototypeName;

                            m_TextureUIAtlas[i].emplace_back(Info);
                            break;
                        }
                    }
                }
                else if(filePath.find(L"Icon") != std::wstring::npos)
                {
                    _wstring UpperFilePath = ToUpper(filePath);
                    for (_uint i = 0; i < (_uint)UIICON::ICON_END; i++)
                    {
                        _wstring IconsName = GetIconName((UIICON)i);
                        if (ContainsExactWord(UpperFilePath, IconsName))
                        {
                            TEXTURE_INFO Info = {};
                            Info.pTexture = pOut;
                            Info.LstrPrototypeName = LSelectTexturePrototypeName;

                            m_TextureUIIcon[i].emplace_back(Info);
                            break;
                        }
                    }
                }
                else if (filePath.find(L"Material") != std::wstring::npos)
                {
                    for (_uint i = 0; i < 450; i++)
                    {
                        TEXTURE_INFO Info = {};
                        Info.pTexture = pOut;
                        Info.LstrPrototypeName = LSelectTexturePrototypeName;
                        m_TextureUIMaterial.emplace_back(Info);
                        break;

                    }
                }
                else if (filePath.find(L"Texture") != std::wstring::npos)
                {
                    _wstring UpperFilePath = ToUpper(filePath);
                    for (_uint i = 0; i < (_uint)TEXTURE::TEXTURE_END; i++)
                    {
                        _wstring TexturesName = GetTextureName((TEXTURE)i);
                        if (ContainsExactWord(UpperFilePath, TexturesName))
                        {
                            TEXTURE_INFO Info = {};
                            Info.pTexture = pOut;
                            Info.LstrPrototypeName = LSelectTexturePrototypeName;

                            m_TextureUITexture[i].emplace_back(Info);
                            break;
                        }
                    }
                }
                       
            }

        }
    } while (FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);  // �ڵ� �ݱ�
}

_uint CLevel_MapTool::GetFileCountFolder(const _wstring& FolderPath)
{
    _uint fileCount = 0;

    _wstring LSearchPath = FolderPath + L"\\*";

    WIN32_FIND_DATAW findData;
    HANDLE hFind = FindFirstFileW(LSearchPath.c_str(), &findData);
 
    do
    {
        if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
            continue;

        std::wstring filePath = FolderPath + L"\\" + findData.cFileName;

        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
        {
            fileCount++;
        }     
    } while (FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind); 
    return fileCount;
}

void CLevel_MapTool::ShowTextures()
{
    if (ImGui::Button((u8"�ε�� UI����")))              
    {   
        // UI �����ϴ� ��ƾ ���, �θ���� ���
        // ��ο��� ��� ����Ƽ���׹����

        m_isShowLoadTexture = true;
    }
    if (ImGui::Button((u8"�ε�� UI�ݱ�")))             
    {
        m_isShowLoadTexture = false;
    }

    
    if (true == m_isShowLoadTexture)
    {
        if (ImGui::TreeNode("UIATLAS List"))            // �̰� �������� ������ ��â ��Ƹ���
        {
            for (_uint i = 0; i < (_uint)UIATLAS::ATLAS_END; ++i)
            {
                string ListName = wstringToString(GetAtlasName((UIATLAS)i).c_str());
                if (ImGui::TreeNode(ListName.c_str()))
                {         
                    for (_uint j = 0; j < m_TextureUIAtlas[i].size(); j++)
                    {                  
                        ID3D11ShaderResourceView* pSRV = m_TextureUIAtlas[i][j].pTexture->GetTexture();  // ���� �ø� �ؽ���
                        ImTextureID textureID = reinterpret_cast<ImTextureID>(pSRV);            // ���� �ø� �ؽ���

                        ImVec2 pos = ImGui::GetCursorScreenPos();                     
                        ImDrawList* drawList = ImGui::GetWindowDrawList();                 
                        drawList->AddRect(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f))); 
                        ImGui::Image(textureID, ImVec2(m_iTextureSizeX, m_iTextureSizeY));
              
                        // �̹��� �������� �巡�� ����
                        if (ImGui::IsItemHovered())
                        {
                            if (true == m_isActiveChangeTexture)
                            {
                                drawList->AddRectFilled(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), IM_COL32(255, 0, 0, 50));

                                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                                {
                                    m_LstrChangeTexturePrototypeName = m_TextureUIAtlas[i][j].LstrPrototypeName;                                 
                                }
                            }
                                
                         

                            if (true == m_isActiveDrag && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                                // �巡�� ���� ��ǥ ����
                                startDrag = ImGui::GetIO().MousePos;
                             
                                // �巡���� LT, RB ��ǥ�� ���� �ؽ�ó�� UV ��ǥ�� ��ȯ
                                ImVec2 itemMin = ImGui::GetItemRectMin(); // �̹����� ���� ��ġ
                                startUV.x = (startDrag.x - itemMin.x) / m_iTextureSizeX;
                                startUV.y = (startDrag.y - itemMin.y) / m_iTextureSizeY;

                                isDragging = true;
                            }
                            if (isDragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                                // �巡�� ���� ���� ���콺 ��ġ�� �巡�� ���� ��ǥ�� ������Ʈ
                                endDrag = ImGui::GetIO().MousePos;

                                // �巡���� LT, RB ��ǥ�� ���� �ؽ�ó�� UV ��ǥ�� ��ȯ
                                ImVec2 itemMin = ImGui::GetItemRectMin(); // �̹����� ���� ��ġ
                                endUV.x = (endDrag.x - itemMin.x) / m_iTextureSizeX;
                                endUV.y = (endDrag.y - itemMin.y) / m_iTextureSizeY;


                                // �巡�� ���� ������ �ð������� ǥ��
                                ImDrawList* drawList = ImGui::GetWindowDrawList();
                                drawList->AddRectFilled(startDrag, endDrag, IM_COL32(255, 0, 0, 50)); // ������ ������
                                drawList->AddRect(startDrag, endDrag, IM_COL32(255, 0, 0, 255)); // ������ �׵θ�
                            }
                            if (isDragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                            {
                                isDragging = false;                                                           
                                
                                // �̶� UV�� �����ؼ� 
                                // �׷� ��ü���� � ��Ʋ�� �̹����� ����ϴ��� �˾ƾ��Ѵ�. (��Ʋ���̹����� ������Ÿ�� �̸��� �˾ƾ��Ѵ�.)
                                
                                // ���� �� ��Ʋ���̹����� UV�� �̾Ѵٶ�� � ��ü���� ����ұ�?
                                 
                                m_vStartUV.x = startUV.x;
                                m_vStartUV.y = startUV.y;
                                m_vEndUV.x = endUV.x;
                                m_vEndUV.y = endUV.y;

                                ImDrawList* drawList = ImGui::GetWindowDrawList();
                                drawList->AddRectFilled(startDrag, endDrag, IM_COL32(255, 0, 0, 50)); // ������ ������
                                drawList->AddRect(startDrag, endDrag, IM_COL32(0, 255, 0, 255)); // ������ �׵θ�
                            }

                        
                            if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                            {
                                startDrag = ImVec2(-1.f, -1.f);
                                endDrag = ImVec2(-1.f, -1.f);
                                startUV = ImVec2(0.f, 0.f);
                                endUV = ImVec2(1.f, 1.f);
                            }
                        }

                            
                    }
                    ImGui::TreePop();
                  }
             
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("UIICON List"))
        {
            for (_uint i = 0; i < (_uint)UIICON::ICON_END; ++i)
            {
                string ListName = wstringToString(GetIconName((UIICON)i).c_str());
                if (ImGui::TreeNode(ListName.c_str()))
                {
                    for (_uint j = 0; j < m_TextureUIIcon[i].size(); j++)
                    {
                        ImTextureID textureID = reinterpret_cast<ImTextureID>(m_TextureUIIcon[i][j].pTexture->GetTexture());
                        ImVec2 pos = ImGui::GetCursorScreenPos();
                        ImDrawList* drawList = ImGui::GetWindowDrawList();                      
                        drawList->AddRect(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)));
                        ImGui::Image(textureID, ImVec2(m_iTextureSizeX, m_iTextureSizeY));

                        if (ImGui::IsItemHovered())
                        {
                            if (true == m_isActiveChangeTexture)
                            {
                                drawList->AddRectFilled(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), IM_COL32(255, 0, 0, 50));

                                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                                {
                                    m_LstrChangeTexturePrototypeName = m_TextureUIIcon[i][j].LstrPrototypeName;
                                }
                            }
                        }

                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("UITexture List"))
        {
            for (_uint i = 0; i < (_uint)TEXTURE::TEXTURE_END; ++i)
            {
                string ListName = wstringToString(GetTextureName((TEXTURE)i).c_str());
                if (ImGui::TreeNode(ListName.c_str()))
                {
                    for (_uint j = 0; j < m_TextureUITexture[i].size(); j++)
                    {
                        ImTextureID textureID = reinterpret_cast<ImTextureID>(m_TextureUITexture[i][j].pTexture->GetTexture());
                        ImVec2 pos = ImGui::GetCursorScreenPos();
                        ImDrawList* drawList = ImGui::GetWindowDrawList();
                        drawList->AddRect(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)));
                        ImGui::Image(textureID, ImVec2(m_iTextureSizeX, m_iTextureSizeY));

                        if (ImGui::IsItemHovered())
                        {
                            if (true == m_isActiveChangeTexture)
                            {
                                drawList->AddRectFilled(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), IM_COL32(255, 0, 0, 50));

                                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                                {
                                    m_LstrChangeTexturePrototypeName = m_TextureUITexture[i][j].LstrPrototypeName;
                                }
                            }
                        }

                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }


        if (ImGui::TreeNode("UIMaterial List"))
        {                   
            if (ImGui::TreeNode("Material"))
            {
                for (_uint i = 0; i < m_TextureUIMaterial.size(); i++)
                {
                    ImTextureID textureID = reinterpret_cast<ImTextureID>(m_TextureUIMaterial[i].pTexture->GetTexture());
                    ImVec2 pos = ImGui::GetCursorScreenPos();
                    ImDrawList* drawList = ImGui::GetWindowDrawList();
                    drawList->AddRect(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)));
                    ImGui::Image(textureID, ImVec2(m_iTextureSizeX, m_iTextureSizeY));

                    if (ImGui::IsItemHovered())
                    {
                        if (true == m_isActiveChangeTexture)
                        {
                            drawList->AddRectFilled(pos, ImVec2(pos.x + m_iTextureSizeX, pos.y + m_iTextureSizeY), IM_COL32(255, 0, 0, 50));

                            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
                                m_LstrChangeTexturePrototypeName = m_TextureUIMaterial[i].LstrPrototypeName;
                            }
                        }
                    }

                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

    }
   



}


ID3D11ShaderResourceView* CLevel_MapTool::CreateCroppedTextureSRV(ID3D11ShaderResourceView* pOriginSRV, _uint iLeft, _uint iTop, _uint iRight, _uint iBottom)
{
    // 1. ���� SRV���� ���ҽ��� ������
    ID3D11Resource* pResource = nullptr;
    pOriginSRV->GetResource(&pResource);

    // 2. ���ҽ��� ID3D11Texture2D�� ĳ����
    ID3D11Texture2D* pOriginalTexture = nullptr;
    HRESULT hr = pResource->QueryInterface(&pOriginalTexture);
    pResource->Release(); // pResource ����

    if (FAILED(hr) || !pOriginalTexture) {
        return nullptr;
    }

    // 3. ���� �ؽ�ó�� ��ũ���͸� ������
    D3D11_TEXTURE2D_DESC desc;
    pOriginalTexture->GetDesc(&desc);

    // �߶� ������ �ʺ�� ���� ���
    UINT width = iRight - iLeft;
    UINT height = iBottom - iTop;

    // 4. �߶� �ؽ�ó�� ��ũ���� ����
    desc.Width = width;
    desc.Height = height;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    // 5. �߶� �ؽ�ó ����
    ID3D11Texture2D* pCroppedTexture = nullptr;
    hr = m_pDevice->CreateTexture2D(&desc, nullptr, &pCroppedTexture);
    if (FAILED(hr) || !pCroppedTexture) {
        pOriginalTexture->Release();
        return nullptr;
    }

    // 6. ���� �ؽ�ó���� �߶� ������ ���ο� �ؽ�ó�� ����
    D3D11_BOX srcBox;
    srcBox.left = iLeft;
    srcBox.top = iTop;
    srcBox.front = 0;
    srcBox.right = iRight;
    srcBox.bottom = iBottom;
    srcBox.back = 1;

    m_pContext->CopySubresourceRegion(pCroppedTexture, 0, 0, 0, 0, pOriginalTexture, 0, &srcBox);

    // 7. �߶� �ؽ�ó�� ���� Shader Resource View ����
    ID3D11ShaderResourceView* pCroppedSRV = nullptr;
    hr = m_pDevice->CreateShaderResourceView(pCroppedTexture, nullptr, &pCroppedSRV);

    pOriginalTexture->Release();
    pCroppedTexture->Release();


    return pCroppedSRV;
}


_wstring CLevel_MapTool::GetAtlasName(UIATLAS atlas)
{
    switch (atlas) {
    case UIATLAS::ACHIEVEMENTS: return L"ACHIEVEMENTS";
    case UIATLAS::ADVENTURE: return L"ADVENTURE";
    case UIATLAS::ADVENTURE_ACT_01: return L"ADVENTURE_ACT_01";
    case UIATLAS::ADVENTURE_ACT_02: return L"ADVENTURE_ACT_02";
    case UIATLAS::ADVENTURE_ACT_03: return L"ADVENTURE_ACT_03";
    case UIATLAS::ADVENTURE_MAIN: return L"ADVENTURE_MAIN";
    case UIATLAS::ALL: return L"ALL";
    case UIATLAS::ALLOUTWAR: return L"ALLOUTWAR";
    case UIATLAS::ATTRIBUTE: return L"ATTRIBUTE";
    case UIATLAS::BG: return L"BG";
    case UIATLAS::BLESSINGTEMPLE: return L"BLESSINGTEMPLE";
    case UIATLAS::CASHSHOP: return L"CASHSHOP";
    case UIATLAS::CHAT: return L"CHAT";
    case UIATLAS::COMMON: return L"COMMON";
    case UIATLAS::COMMON_FULLSCREEN: return L"COMMON_FULLSCREEN";
    case UIATLAS::CONSTRUCT: return L"CONSTRUCT";
    case UIATLAS::CONSUMEBAG: return L"CONSUMEBAG";
    case UIATLAS::COOK: return L"COOK";
    case UIATLAS::COSTUME: return L"COSTUME";
    case UIATLAS::CUSTOMIZE: return L"CUSTOMIZE";
    case UIATLAS::CUSTOMIZE_PRESET: return L"CUSTOMIZE_PRESET";
    case UIATLAS::DEAD: return L"DEAD";
    case UIATLAS::DELIVERY: return L"DELIVERY";
    case UIATLAS::DMG: return L"DMG";
    case UIATLAS::DUNGEON: return L"DUNGEON";
    case UIATLAS::EMOJI: return L"EMOJI";
    case UIATLAS::EQUIP: return L"EQUIP";
    case UIATLAS::EVENT: return L"EVENT";
    case UIATLAS::FACTION: return L"FACTION";
    case UIATLAS::FACTIONDUNGEON: return L"FACTIONDUNGEON";
    case UIATLAS::FACTIONELECTION: return L"FACTIONELECTION";
    case UIATLAS::FACTIONMANDATE: return L"FACTIONMANDATE";
    case UIATLAS::FACTIONSPEAKER: return L"FACTIONSPEAKER";
    case UIATLAS::FIELDBOSS: return L"FIELDBOSS";
    case UIATLAS::FISH: return L"FISH";
    case UIATLAS::FORGE: return L"FORGE";
    case UIATLAS::FRONTIERWAR: return L"FRONTIERWAR";
    case UIATLAS::GUILD: return L"GUILD";
    case UIATLAS::GUILD_TERRITORY_AGO: return L"GUILD_TERRITORY_AGO";
    case UIATLAS::GUILD_TERRITORY_ARTHDAL: return L"GUILD_TERRITORY_ARTHDAL";
    case UIATLAS::GUILD_TERRITORY_LAWLESS: return L"GUILD_TERRITORY_LAWLESS";
    case UIATLAS::GUILDCREST_LARGE: return L"GUILDCREST_LARGE";
    case UIATLAS::GUILDCREST_SMALL: return L"GUILDCREST_SMALL";
    case UIATLAS::HISTORYBOOK: return L"HISTORYBOOK";
    case UIATLAS::HUD: return L"HUD";
    case UIATLAS::HUD_ALLOUTWAR: return L"HUD_ALLOUTWAR"; 
    case UIATLAS::HUD_FACTIONWAR: return L"HUD_FACTIONWAR";
    case UIATLAS::ITEMBG: return L"ITEMBG";
    case UIATLAS::ITEMRESTORE: return L"ITEMRESTORE";
    case UIATLAS::LIFESKILL: return L"LIFESKILL";
    case UIATLAS::LOADING: return L"LOADING";
    case UIATLAS::LOBBY: return L"LOBBY";
    case UIATLAS::LOGIN: return L"LOGIN";
    case UIATLAS::MENU: return L"MENU";
    case UIATLAS::MERCENARYQUEST: return L"MERCENARYQUEST";
    case UIATLAS::MINIMAP: return L"MINIMAP";
    case UIATLAS::MONSTER: return L"MONSTER";
    case UIATLAS::OPTION: return L"OPTION";
    case UIATLAS::PARTY: return L"PARTY";
    case UIATLAS::PKMENU: return L"PKMENU";
    case UIATLAS::PORTRAIT: return L"PORTRAIT";
    case UIATLAS::PUZZLE: return L"PUZZLE";
    case UIATLAS::QTE: return L"QTE";
    case UIATLAS::QUEST: return L"QUEST";
    case UIATLAS::RESYNTHESLS: return L"RESYNTHESLS";
    case UIATLAS::RIDE: return L"RIDE";
    case UIATLAS::SHORTCUT: return L"SHORTCUT";
    case UIATLAS::SKILL: return L"SKILL";
    case UIATLAS::SPIRIT: return L"SPIRIT";
    case UIATLAS::WEAPON: return L"WEAPON";
    case UIATLAS::WORLDMAP: return L"WORLDMAP";
    case UIATLAS::ZONESPAWN: return L"ZONESPAWN";
    default: return L"";
    }
}

_wstring CLevel_MapTool::GetIconName(UIICON Icon)
{
    switch (Icon) {
    case UIICON::ACC: return L"ACC";
    case UIICON::ARMOR: return L"ARMOR";
    case UIICON::BM: return L"BM";
    case UIICON::COSTUME: return L"COSTUME";
    case UIICON::CURRENCY: return L"CURRENCY";
    case UIICON::ITEM: return L"ITEM";
    case UIICON::ITEM_COLLECTIONOBJ: return L"ITEM_COLLECTIONOBJ";
    case UIICON::ITEM_FISH: return L"ITEM_FISH";
    case UIICON::ITEM_SKILLBOOK: return L"ITEM_SKILLBOOK";
    case UIICON::RIDE: return L"RIDE";
    case UIICON::SKILL: return L"SKILL";
    case UIICON::SPIRIT: return L"SPIRIT";
    case UIICON::WEAPON: return L"WEAPON";
    default: return L"";
    }
}

_wstring CLevel_MapTool::GetTextureName(TEXTURE Texture)
{
    switch (Texture) {
    case TEXTURE::ADVENTURE: return L"ADVENTURE";
    case TEXTURE::ALLOUTWAR: return L"ALLOUTWAR";
    case TEXTURE::BANNER: return L"BANNER";
    case TEXTURE::BG: return L"BG";
    case TEXTURE::BLESSINGTEMPLE: return L"BLESSINGTEMPLE";
    case TEXTURE::CIBI: return L"CIBI";
    case TEXTURE::COSTUME: return L"COSTUME";
    case TEXTURE::CUTSCENE: return L"CUTSCENE";
    case TEXTURE::DEV: return L"DEV";
    case TEXTURE::DEVICESTATUS: return L"DEVICESTATUS";
    case TEXTURE::EMOJI: return L"EMOJI";
    case TEXTURE::ETC: return L"ETC";
    case TEXTURE::EVENT: return L"EVENT";
    case TEXTURE::FACTION: return L"FACTION";
    case TEXTURE::GUIDE: return L"GUIDE";
    case TEXTURE::GUILD: return L"GUILD";
    case TEXTURE::HISTORYBOOK: return L"HISTORYBOOK";
    case TEXTURE::ICONBG: return L"ICONBG";
    case TEXTURE::INTEGRATIONFIELD: return L"INTEGRATIONFIELD";
    case TEXTURE::LARGEDECO: return L"LARGEDECO";
    case TEXTURE::LOADING: return L"LOADING";
    case TEXTURE::MAP: return L"MAP";
    case TEXTURE::MERCENARYQUEST: return L"MERCENARYQUEST";
    case TEXTURE::PORTRAIT: return L"PORTRAIT";
    case TEXTURE::RIDING: return L"RIDING";
    case TEXTURE::SERVERTRANSFER: return L"SERVERTRANSFER";
    case TEXTURE::SHOP: return L"SHOP";
    case TEXTURE::SPIRIT: return L"SPIRIT";
    case TEXTURE::TILING: return L"TILING";
    case TEXTURE::TIMEDUNGEON: return L"TIMEDUNGEON";
    case TEXTURE::TUTORIAL: return L"TUTORIAL";
    default: return L"";
    }
}

_bool CLevel_MapTool::ContainsExactWord(const wstring& filePath, const wstring& word)
{
    size_t iPos = filePath.find(word);

    if (iPos != wstring::npos)
    {
        if (filePath[iPos + word.length()] == L'\\' || filePath[iPos + word.length()] == L'/')
            return true;
        else
            return false;
    }     
    else
        return false;
}


void CLevel_MapTool::Save_DefalutUI()
{
    FILE* fout = fopen("../Bin/DefalutUI.bin", "wb");
    if (!fout)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    list<CGameObject*>& UILists = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_UI"));
    _uint iUISize = UILists.size();
    fwrite(&iUISize, sizeof(iUISize), 1, fout);
    for (auto pObj : UILists)
    {
        CUI* pUI = static_cast<CUI*>(pObj);
        Save_DefalutUIWithChild(pUI, fout);            
    }



    fclose(fout);
    MSG_BOX(TEXT("Defalut UI ���� ����!"));
}

void CLevel_MapTool::Save_BatchUI()
{
    list<CGameObject*>& BatchUIs = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_UIBatch"));  // �� �ӿ� ���� UI���� ������ �˾ƾ��Ѵ� ���������Ѵ� ! �����������ϴϱ�  
    _uint iBatchUISize = BatchUIs.size();   
    if (iBatchUISize == 0)
    {
        MSG_BOX(TEXT("��ġ�� UI�� �����ϴ� ��ġ������ �Ϸ��ߴ��� ���� üũ�ϼ���"));
        return;
    }

    list<CGameObject*>& ForBatchUIs = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_UI_ForBatch"));  
    _uint iForBatchUISize = ForBatchUIs.size();
    if (iForBatchUISize > 0)
    {
        MSG_BOX(TEXT("���� ��Ī�� �Ϸ���� ���� Layer_UI_ForBatch��ü�� �����մϴ�\n�ٽ� Ȯ���ϼ�"));
        return;
    }

    _wstring LastMsg = L"���� ��ġUI�� �����Ͻǰǰ���?\nPŰ�� ��ġUI�����ߴ��� ���������� Ȯ����\n���� �����Ϸ��� Layer_UIBatch�ǰ���:>>>>>" + to_wstring(iBatchUISize);

    _int iResult = MessageBox(
        NULL,                             
        LastMsg.c_str(),    
        TEXT("����"),                       // �޽��� ����
        MB_OKCANCEL | MB_ICONQUESTION      
    );

    if (iResult == IDCANCEL)
    {
        MessageBox(NULL, TEXT("���߿� �ٽ� �õ��ϼ���"), TEXT("������"), MB_OK);
        return;
    }


    FILE* fout = fopen("../Bin/BatchUI.bin", "wb");
    if (!fout)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }

    fwrite(&iBatchUISize, sizeof(iBatchUISize), 1, fout);       // ��ġ �׷��� ��� ?

    for (auto pObj : BatchUIs)
    {
        CUIBatch* pBatchUI = static_cast<CUIBatch*>(pObj);
        list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = pBatchUI->Get_UIList();


        _uint iBatchingGroup = pBatchUI->Get_MyBatchingGroup();
        fwrite(&iBatchingGroup, sizeof(iBatchingGroup), 1, fout);

        _uint iUISize = UIList.size();
        fwrite(&iUISize, sizeof(iUISize), 1, fout);             // �� ��ġ�׷쿡 ���� UI�� ��� ?

        for (auto pObj : UIList)
        {
            CUI* pUI = static_cast<CUI*>(pObj.pUI);

            _wstring LstrPrototypeName = pUI->Get_PrototypeName();          // ��ü�� ����Ÿ��
            _uint PrototypeLength = LstrPrototypeName.size();               // ��ü�� ����Ÿ��

            _wstring LstrLayerName = pUI->Get_LayerName();                  // ��ü�� ���̾�
            _uint LayerLength = LstrLayerName.size();                       // ��ü�� ���̾�

            _float4x4   WorldMatrix = *pUI->Get_Transform()->Get_WorldMatrix_Ptr();

            _wstring LstrMyTexturePath = static_cast<CTexture*>(pUI->Find_Component(L"Com_Texture"))->Get_MyPath();    // �� ��ü�� �����ϴ� �ؽ����� ���
            _uint TexturePathLength = LstrMyTexturePath.size();

            _wstring LstrMyTexturePrototypeName = pUI->Get_MyTexturePrototypeName();    // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��
            _uint TexturePrototypeLength = LstrMyTexturePrototypeName.size();           // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��

            _float2 vStartUV = pUI->Get_StartUV();
            _float2 vEndUV = pUI->Get_EndUV();
            _float2 vTrigerTargetStartUV = pUI->Get_TrigerTargetStartUV();
            _float2 vTrigerTargetEndUV = pUI->Get_TrigerTargetEndUV();

            CUI::FONT_INFO& FontInfo = pUI->Get_FontInfo_Ref();
            _wstring LstrFontText = FontInfo.strFontText;
            _uint    iFontTextLength = LstrFontText.size();
            _float4  vFontColor = FontInfo.vFontColor;



            fwrite(&PrototypeLength, sizeof(PrototypeLength), 1, fout);                  // ����Ÿ��
            fwrite(LstrPrototypeName.c_str(), sizeof(_tchar), PrototypeLength, fout);    // ����Ÿ��

            fwrite(&LayerLength, sizeof(LayerLength), 1, fout);                          // ���̾�
            fwrite(LstrLayerName.c_str(), sizeof(_tchar), LayerLength, fout);            // ���̾�

            fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);

            fwrite(&TexturePathLength, sizeof(TexturePathLength), 1, fout);
            fwrite(LstrMyTexturePath.c_str(), sizeof(_tchar), TexturePathLength, fout);

            fwrite(&TexturePrototypeLength, sizeof(TexturePrototypeLength), 1, fout);
            fwrite(LstrMyTexturePrototypeName.c_str(), sizeof(_tchar), TexturePrototypeLength, fout);

            fwrite(&vStartUV, sizeof(vStartUV), 1, fout);
            fwrite(&vEndUV, sizeof(vEndUV), 1, fout);
            fwrite(&vTrigerTargetStartUV, sizeof(vTrigerTargetStartUV), 1, fout);
            fwrite(&vTrigerTargetEndUV, sizeof(vTrigerTargetEndUV), 1, fout);

            fwrite(&iFontTextLength, sizeof(iFontTextLength), 1, fout);
            fwrite(LstrFontText.c_str(), sizeof(_tchar), iFontTextLength, fout);
            fwrite(&vFontColor, sizeof(vFontColor), 1, fout);
        }
    }



    
    fclose(fout);
    MSG_BOX(TEXT("Batch UI ���� ����!"));
}


void CLevel_MapTool::Save_SkillMenuUI()
{
    FILE* fout = fopen("../Bin/SkillMenuUI.bin", "wb");
    if (!fout)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� ���⸦ �����߾��.."));
        return;
    }


    list<CGameObject*>& UILists = m_pGameInstance->Get_GameObjects(LEVEL_MAPTOOL, TEXT("Layer_UI"));
    _uint iUISize = UILists.size();
    fwrite(&iUISize, sizeof(iUISize), 1, fout);
    for (auto pObj : UILists)
    {
        CUI* pUI = static_cast<CUI*>(pObj);
        Save_DefalutUIWithChild(pUI, fout);
    }



    fclose(fout);
    MSG_BOX(TEXT("SkillMenuUI UI ���� ����!"));
}


void CLevel_MapTool::Load_SkillMenuUI()
{
    FILE* fin = fopen("../Bin/SkillMenuUI.bin", "rb");
    if (!fin)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
        return;
    }

    _uint iUISize;
    fread(&iUISize, sizeof(iUISize), 1, fin);

    for (_uint i = 0; i < iUISize; ++i)
    {
        CUI* pParentUI = Load_DefalutUIWithChild(fin);
        if (nullptr == pParentUI)       // ��ġUI ���ٴ¼Ҹ��� 
        {

        }
        else                            // �ֻ��� �θ�UI �̰�
        {
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, pParentUI->Get_LayerName(), pParentUI)));     // �̰� �纻�� ������ִ°Ŵϱ�   pParentUI�� �� ���̾ �߰����ϸ�� 
        }


    }

    fclose(fin);
    MSG_BOX(TEXT("SkillMenu UI �ε� ����!"));
}


void CLevel_MapTool::Save_DefalutUIWithChild(CUI* pUI, FILE* fout)
{
    _wstring LstrPrototypeName = pUI->Get_PrototypeName();          // ��ü�� ����Ÿ��
    _uint PrototypeLength = LstrPrototypeName.size();               // ��ü�� ����Ÿ��

    _wstring LstrLayerName = pUI->Get_LayerName();                  // ��ü�� ���̾�
    _uint LayerLength = LstrLayerName.size();                       // ��ü�� ���̾�

    _float4x4   WorldMatrix = *pUI->Get_Transform()->Get_WorldMatrix_Ptr();

    _wstring LstrMyTexturePath = static_cast<CTexture*>(pUI->Find_Component(L"Com_Texture"))->Get_MyPath();    // �� ��ü�� �����ϴ� �ؽ����� ���
    _uint TexturePathLength = LstrMyTexturePath.size();

    _wstring LstrMyTexturePrototypeName = pUI->Get_MyTexturePrototypeName();    // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��
    _uint TexturePrototypeLength = LstrMyTexturePrototypeName.size();           // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��

    _float2 vStartUV = pUI->Get_StartUV();
    _float2 vEndUV = pUI->Get_EndUV();
    _float2 vTrigerTargetStartUV = pUI->Get_TrigerTargetStartUV();
    _float2 vTrigerTargetEndUV = pUI->Get_TrigerTargetEndUV();

    CUI::FONT_INFO& FontInfo = pUI->Get_FontInfo_Ref();
    _wstring LstrFontText = FontInfo.strFontText;      
    _uint    iFontTextLength = LstrFontText.size();
    _float4  vFontColor = FontInfo.vFontColor;
    
   
    fwrite(&PrototypeLength, sizeof(PrototypeLength), 1, fout);                  // ����Ÿ��
    fwrite(LstrPrototypeName.c_str(), sizeof(_tchar), PrototypeLength, fout);    // ����Ÿ��

    fwrite(&LayerLength, sizeof(LayerLength), 1, fout);                          // ���̾�
    fwrite(LstrLayerName.c_str(), sizeof(_tchar), LayerLength, fout);            // ���̾�

    fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);

    fwrite(&TexturePathLength, sizeof(TexturePathLength), 1, fout);
    fwrite(LstrMyTexturePath.c_str(), sizeof(_tchar), TexturePathLength, fout);

    fwrite(&TexturePrototypeLength, sizeof(TexturePrototypeLength), 1, fout);
    fwrite(LstrMyTexturePrototypeName.c_str(), sizeof(_tchar), TexturePrototypeLength, fout);

    fwrite(&vStartUV, sizeof(vStartUV), 1, fout);
    fwrite(&vEndUV, sizeof(vEndUV), 1, fout);
    fwrite(&vTrigerTargetStartUV, sizeof(vTrigerTargetStartUV), 1, fout);
    fwrite(&vTrigerTargetEndUV, sizeof(vTrigerTargetEndUV), 1, fout);


    fwrite(&iFontTextLength, sizeof(iFontTextLength), 1, fout);
    fwrite(LstrFontText.c_str(), sizeof(_tchar), iFontTextLength, fout);
    fwrite(&vFontColor, sizeof(vFontColor), 1, fout);
    
    

    
    // ������ üũ�Ұ� ���� ���� ��ġUI���ٸ�?
    //  ----------- �ڱ��ڽ� �����ߴٸ� ���鼭 �ڽ��� �������� -----------     

    list<CUI*>& ChildUIList = pUI->GetChildUI();
    _uint iChildSize = ChildUIList.size();
    fwrite(&iChildSize, sizeof(iChildSize), 1, fout);

    for (auto pUI : ChildUIList)
    {
        _wstring LstrPrototypeName = pUI->Get_PrototypeName();          // ��ü�� ����Ÿ��
        _uint PrototypeLength = LstrPrototypeName.size();               // ��ü�� ����Ÿ��

        _wstring LstrLayerName = pUI->Get_LayerName();                  // ��ü�� ���̾�
        _uint LayerLength = LstrLayerName.size();                       // ��ü�� ���̾�

        _float4x4   WorldMatrix = *pUI->Get_Transform()->Get_WorldMatrix_Ptr();

        _wstring LstrMyTexturePath = static_cast<CTexture*>(pUI->Find_Component(L"Com_Texture"))->Get_MyPath();    // �� ��ü�� �����ϴ� �ؽ����� ���       
        _uint TexturePathLength = LstrMyTexturePath.size();

        _wstring LstrMyTexturePrototypeName = pUI->Get_MyTexturePrototypeName();    // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��
        _uint TexturePrototypeLength = LstrMyTexturePrototypeName.size();           // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��

        _float2 vStartUV = pUI->Get_StartUV();
        _float2 vEndUV = pUI->Get_EndUV();
        _float2 vTrigerTargetStartUV = pUI->Get_TrigerTargetStartUV();
        _float2 vTrigerTargetEndUV = pUI->Get_TrigerTargetEndUV();
       
        CUI::FONT_INFO& FontInfo = pUI->Get_FontInfo_Ref();
        _wstring LstrFontText = FontInfo.strFontText;
        _uint    iFontTextLength = LstrFontText.size();
        _float4  vFontColor = FontInfo.vFontColor;

        fwrite(&PrototypeLength, sizeof(PrototypeLength), 1, fout);                  // ����Ÿ��
        fwrite(LstrPrototypeName.c_str(), sizeof(_tchar), PrototypeLength, fout);    // ����Ÿ��

        fwrite(&LayerLength, sizeof(LayerLength), 1, fout);                          // ���̾�
        fwrite(LstrLayerName.c_str(), sizeof(_tchar), LayerLength, fout);            // ���̾�

        fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);

        fwrite(&TexturePathLength, sizeof(TexturePathLength), 1, fout);
        fwrite(LstrMyTexturePath.c_str(), sizeof(_tchar), TexturePathLength, fout);

        fwrite(&TexturePrototypeLength, sizeof(TexturePrototypeLength), 1, fout);
        fwrite(LstrMyTexturePrototypeName.c_str(), sizeof(_tchar), TexturePrototypeLength, fout);

        fwrite(&vStartUV, sizeof(vStartUV), 1, fout);
        fwrite(&vEndUV, sizeof(vEndUV), 1, fout);
        fwrite(&vTrigerTargetStartUV, sizeof(vTrigerTargetStartUV), 1, fout);
        fwrite(&vTrigerTargetEndUV, sizeof(vTrigerTargetEndUV), 1, fout);

        fwrite(&iFontTextLength, sizeof(iFontTextLength), 1, fout);
        fwrite(LstrFontText.c_str(), sizeof(_tchar), iFontTextLength, fout);
        fwrite(&vFontColor, sizeof(vFontColor), 1, fout);

        if (L"Layer_UIBatch" == LstrLayerName)                                    // ��Ī UI���ٸ� 
        {
            CUIBatch* pBatchUI = dynamic_cast<CUIBatch*>(pUI);

            list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = pBatchUI->Get_UIList();
            _uint iBatchingGroup = pBatchUI->Get_MyBatchingGroup();
            fwrite(&iBatchingGroup, sizeof(iBatchingGroup), 1, fout);

            _uint iUISize = UIList.size();
            fwrite(&iUISize, sizeof(iUISize), 1, fout);             // �� ��ġ�׷쿡 ���� UI�� ��� ?

            for (auto pObj : UIList)
            {
                CUI* pUI = static_cast<CUI*>(pObj.pUI);

                _wstring LstrPrototypeName = pUI->Get_PrototypeName();          // ��ü�� ����Ÿ��
                _uint PrototypeLength = LstrPrototypeName.size();               // ��ü�� ����Ÿ��

                _wstring LstrLayerName = pUI->Get_LayerName();                  // ��ü�� ���̾�
                _uint LayerLength = LstrLayerName.size();                       // ��ü�� ���̾�

                _float4x4   WorldMatrix = *pUI->Get_Transform()->Get_WorldMatrix_Ptr();

                _wstring LstrMyTexturePath = static_cast<CTexture*>(pUI->Find_Component(L"Com_Texture"))->Get_MyPath();  // �� ��ü�� �����ϴ� �ؽ����� ���
               
                _uint TexturePathLength = LstrMyTexturePath.size();

                _wstring LstrMyTexturePrototypeName = pUI->Get_MyTexturePrototypeName();    // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��
               
                _uint TexturePrototypeLength = LstrMyTexturePrototypeName.size();           // �� ��ü�� �����ϴ� �ؽ����� ����Ÿ��

                _float2 vStartUV = pUI->Get_StartUV();
                _float2 vEndUV = pUI->Get_EndUV();
                _float2 vTrigerTargetStartUV = pUI->Get_TrigerTargetStartUV();
                _float2 vTrigerTargetEndUV = pUI->Get_TrigerTargetEndUV();

                CUI::FONT_INFO& FontInfo = pUI->Get_FontInfo_Ref();
                _wstring LstrFontText = FontInfo.strFontText;
                _uint    iFontTextLength = LstrFontText.size();
                _float4  vFontColor = FontInfo.vFontColor;

                fwrite(&PrototypeLength, sizeof(PrototypeLength), 1, fout);                  // ����Ÿ��
                fwrite(LstrPrototypeName.c_str(), sizeof(_tchar), PrototypeLength, fout);    // ����Ÿ��

                fwrite(&LayerLength, sizeof(LayerLength), 1, fout);                          // ���̾�
                fwrite(LstrLayerName.c_str(), sizeof(_tchar), LayerLength, fout);            // ���̾�

                fwrite(&WorldMatrix, sizeof(WorldMatrix), 1, fout);

                fwrite(&TexturePathLength, sizeof(TexturePathLength), 1, fout);
                fwrite(LstrMyTexturePath.c_str(), sizeof(_tchar), TexturePathLength, fout);

                fwrite(&TexturePrototypeLength, sizeof(TexturePrototypeLength), 1, fout);
                fwrite(LstrMyTexturePrototypeName.c_str(), sizeof(_tchar), TexturePrototypeLength, fout);

                fwrite(&vStartUV, sizeof(vStartUV), 1, fout);
                fwrite(&vEndUV, sizeof(vEndUV), 1, fout);
                fwrite(&vTrigerTargetStartUV, sizeof(vTrigerTargetStartUV), 1, fout);
                fwrite(&vTrigerTargetEndUV, sizeof(vTrigerTargetEndUV), 1, fout);
                
                fwrite(&iFontTextLength, sizeof(iFontTextLength), 1, fout);
                fwrite(LstrFontText.c_str(), sizeof(_tchar), iFontTextLength, fout);
                fwrite(&vFontColor, sizeof(vFontColor), 1, fout);

            }
        }
       
        list<CUI*>& ChildUIList = pUI->GetChildUI();
        _uint iChildSize = ChildUIList.size();
        fwrite(&iChildSize, sizeof(iChildSize), 1, fout);

        // �� �ڽ��� ��������� ����
        for (auto pChildUI : ChildUIList)
        {
            Save_DefalutUIWithChild(pChildUI, fout); // ��� ȣ��� �ڽĵ� ����
        }

    }
}

void CLevel_MapTool::Load_DefalutUI()
{
    FILE* fin = fopen("../Bin/DefalutUI.bin", "rb");
    if (!fin)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
        return;
    }

    _uint iUISize;
    fread(&iUISize, sizeof(iUISize), 1, fin);

    for (_uint i = 0; i < iUISize; ++i)
    {
        CUI* pParentUI = Load_DefalutUIWithChild(fin);
        if (nullptr == pParentUI)       // ��ġUI ���ٴ¼Ҹ��� 
        {
            
        }
        else                            // �ֻ��� �θ�UI �̰�
        {
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, pParentUI->Get_LayerName(), pParentUI)));     // �̰� �纻�� ������ִ°Ŵϱ�   pParentUI�� �� ���̾ �߰����ϸ�� 
        }

        
    }

    fclose(fin);
    MSG_BOX(TEXT("Defalut UI �ε� ����!"));
}

CUI* CLevel_MapTool::Load_DefalutUIWithChild(FILE* fin)
{
    _wstring LstrPrototypeName = L"";
    _wstring LstrLayerName = L"";

    _float4x4   WorldMatrix = {};
    _float2 vStartUV, vEndUV;
    _float2 vTrigerTargetStartUV, vTrigerTargetEndUV;

    _wstring LstrMyTexturePath = L"";
    _wstring LstrMyTexturePrototypeName = L"";

    CUI::FONT_INFO FontInfo = {};

    ReadString(fin, LstrPrototypeName);
    ReadString(fin, LstrLayerName);

    fread(&WorldMatrix, sizeof(WorldMatrix), 1, fin);

    ReadString(fin, LstrMyTexturePath);
    ReadString(fin, LstrMyTexturePrototypeName);

    fread(&vStartUV, sizeof(vStartUV), 1, fin);
    fread(&vEndUV, sizeof(vEndUV), 1, fin);
    fread(&vTrigerTargetStartUV, sizeof(vTrigerTargetStartUV), 1, fin);
    fread(&vTrigerTargetEndUV, sizeof(vTrigerTargetEndUV), 1, fin);

    ReadString(fin, FontInfo.strFontText);
    fread(&FontInfo.vFontColor, sizeof(FontInfo.vFontColor), 1, fin);


    _vector worldPosition = XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2], 1.0f);
    _matrix viewProjMatrix = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixOrthographicLH(1600.f, 900.f, 0.f, 1.f));
    _vector screenPosition = XMVector3TransformCoord(worldPosition, viewProjMatrix);

    _float fScreenX = XMVectorGetX(screenPosition) * 1600.f * 0.5f + 1600.f * 0.5f;
    _float fScreenY = -XMVectorGetY(screenPosition) * 900.f * 0.5f + 900.f * 0.5f;


    _vector vRight = XMLoadFloat3(&_float3(WorldMatrix.m[0][0], WorldMatrix.m[0][1], WorldMatrix.m[0][2]));
    _vector vUp = XMLoadFloat3(&_float3(WorldMatrix.m[1][0], WorldMatrix.m[1][1], WorldMatrix.m[1][2]));

    _float fSizeX = XMVectorGetX(XMVector3Length(vRight));       //  ��Ʈ x^2 + y^2 + z^2 �� �����Ǳ��̴ϱ� 
    _float fSizeY = XMVectorGetX(XMVector3Length(vUp));


    CUI* pUI = nullptr;

    if (LstrLayerName == L"Layer_UIBatch")                     // ��Ī UI���ٸ� 
    {
        _uint iBatchingGroup = {};
        fread(&iBatchingGroup, sizeof(iBatchingGroup), 1, fin);

        _uint iUISize;
        fread(&iUISize, sizeof(iUISize), 1, fin);              // �� ��ġ�� UI�� ��� 

        for (_uint i = 0; i < iUISize; ++i)
        {
            _wstring LstrPrototypeName = L"";
            _wstring LstrLayerName = L"";

            _float4x4   WorldMatrix = {};
            _float2 vStartUV, vEndUV;
            _float2 vTrigerTargetStartUV;
            _float2 vTrigerTargetEndUV;

            _wstring LstrMyTexturePath = L"";
            _wstring LstrMyTexturePrototypeName = L"";

            CUI::FONT_INFO FontInfo = {};

            // ����� ������ ������� �о�ɴϴ�.

            ReadString(fin, LstrPrototypeName);
            ReadString(fin, LstrLayerName);

            fread(&WorldMatrix, sizeof(WorldMatrix), 1, fin);

            ReadString(fin, LstrMyTexturePath);
            ReadString(fin, LstrMyTexturePrototypeName);

            fread(&vStartUV, sizeof(vStartUV), 1, fin);
            fread(&vEndUV, sizeof(vEndUV), 1, fin);
            fread(&vTrigerTargetStartUV, sizeof(vStartUV), 1, fin);
            fread(&vTrigerTargetEndUV, sizeof(vEndUV), 1, fin);

            ReadString(fin, FontInfo.strFontText);
            fread(&FontInfo.vFontColor, sizeof(FontInfo.vFontColor), 1, fin);

            _vector worldPosition = XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2], 1.0f);
            _matrix viewProjMatrix = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixOrthographicLH(1600.f, 900.f, 0.f, 1.f));
            _vector screenPosition = XMVector3TransformCoord(worldPosition, viewProjMatrix);

            _float fScreenX = XMVectorGetX(screenPosition) * 1600.f * 0.5f + 1600.f * 0.5f;
            _float fScreenY = -XMVectorGetY(screenPosition) * 900.f * 0.5f + 900.f * 0.5f;


            _vector vRight = XMLoadFloat3(&_float3(WorldMatrix.m[0][0], WorldMatrix.m[0][1], WorldMatrix.m[0][2]));
            _vector vUp = XMLoadFloat3(&_float3(WorldMatrix.m[1][0], WorldMatrix.m[1][1], WorldMatrix.m[1][2]));

            _float fSizeX = XMVectorGetX(XMVector3Length(vRight));       //  ��Ʈ x^2 + y^2 + z^2 �� �����Ǳ��̴ϱ� 
            _float fSizeY = XMVectorGetX(XMVector3Length(vUp));


            CUI::UI_DESC		Desc{};
            Desc.fX = fScreenX;
            Desc.fY = fScreenY;
            Desc.fSizeX = fSizeX;
            Desc.fSizeY = fSizeY;
            Desc.LstrTexturePrototype = LstrMyTexturePrototypeName;
            Desc.vStartUV = vStartUV;
            Desc.vEndUV = vEndUV;
            Desc.vTrigerTargetStartUV = vTrigerTargetStartUV;
            Desc.vTrigerTargetEndUV = vTrigerTargetEndUV;

            Desc.fSpeedPerSec = 10.f;
            Desc.fRotationPerSec = XMConvertToRadians(90.0f);
            Desc.pBatchObj_InitWorldMatrix = &WorldMatrix;
            Desc.FontInfo = FontInfo;
            
            // �����󿡼� ��¥�� ��� �̹����� �ε��ҰŶ� ��������� ����Ŭ�󿡼� �纻�������� ������ �ؽ��� ���� �������ߤ��Ѵ�
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, LstrLayerName,
                LstrPrototypeName, &Desc)))
                MSG_BOX(L"�ε�� ������ �ȳ���");
        }

        CUIBatch::BATCH_GROUP_DESC Desc = {};
        Desc.eGroup = (CUIBatch::BATHCING_GROUP)iBatchingGroup;

        pUI = static_cast<CUI*>(m_pGameInstance->Clone_GameObject(LstrPrototypeName, &Desc));
        pUI->Set_LayerName(L"Layer_UIBatch");
        //m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_UIBatch", pUI);
    }
    else
    {
        CUI::UI_DESC		Desc{};
        Desc.fX = fScreenX;
        Desc.fY = fScreenY;
        Desc.fSizeX = fSizeX;
        Desc.fSizeY = fSizeY;
        Desc.LstrTexturePrototype = LstrMyTexturePrototypeName;
        Desc.vStartUV = vStartUV;
        Desc.vEndUV = vEndUV;
        Desc.vTrigerTargetStartUV = vTrigerTargetStartUV;
        Desc.vTrigerTargetEndUV = vTrigerTargetEndUV;
        Desc.fSpeedPerSec = 10.f;
        Desc.fRotationPerSec = XMConvertToRadians(90.0f);
        Desc.FontInfo = FontInfo;

        // �����󿡼� ��¥�� ��� �̹����� �ε��ҰŶ� ��������� ����Ŭ�󿡼� �纻�������� ������ �ؽ��� ���� �������ߤ��Ѵ�
        pUI = static_cast<CUI*>(m_pGameInstance->Clone_GameObject(LstrPrototypeName, &Desc));
        pUI->Set_PrototypeName(LstrPrototypeName);
        pUI->Set_LayerName(LstrLayerName);
    }
   


    _uint iChildUISize;
    fread(&iChildUISize, sizeof(iChildUISize), 1, fin);
    for (_uint i = 0; i < iChildUISize; i++)
    {
        CUI*            pChildUI = Load_DefalutUIWithChild(fin);
        list<CUI*>&     ChildUIList = pUI->GetChildUI();
        ChildUIList.emplace_back(pChildUI);
      
        if (L"Layer_UIBatch" == pChildUI->Get_LayerName())
        {
            list<CVIBuffer_RectBatch::UIBATCH_INFO>& UIList = static_cast<CUIBatch*>(pChildUI)->Get_UIList();
            for (auto UIBatchInfo : UIList)
            {
                UIBatchInfo.pUI->SetParent(pUI);
            }
            pChildUI->SetParent(pUI);
        }
        else
            pChildUI->SetParent(pUI);
    }

    return pUI;
}

void CLevel_MapTool::Load_BatchUI()
{
    FILE* fin = fopen("../Bin/BatchUI.bin", "rb");
    if (!fin)  // ������ ���� ���ߴٸ�
    {
        MSG_BOX(TEXT("���� �б⸦ �����߾��.."));
        return;
    }

    _uint iBatchUISize = {};
    fread(&iBatchUISize, sizeof(iBatchUISize), 1, fin);        // ��ġ �׷��� ��� ?

    for (_uint i = 0; i < iBatchUISize; ++i)
    {
        _uint iBatchingGroup = {};
        fread(&iBatchingGroup, sizeof(iBatchingGroup), 1, fin);


        _uint iUISize;
        fread(&iUISize, sizeof(iUISize), 1, fin);              // �� ��ġ�� UI�� ��� 

        for (_uint i = 0; i < iUISize; ++i)
        {
            _wstring LstrPrototypeName = L"";
            _wstring LstrLayerName = L"";

            _float4x4   WorldMatrix = {};
            _float2 vStartUV, vEndUV;
            _float2 vTrigerTargetStartUV;
            _float2 vTrigerTargetEndUV;

            _wstring LstrMyTexturePath = L"";
            _wstring LstrMyTexturePrototypeName = L"";

            CUI::FONT_INFO FontInfo = {};


            // ����� ������ ������� �о�ɴϴ�.

            ReadString(fin, LstrPrototypeName);
            ReadString(fin, LstrLayerName);

            fread(&WorldMatrix, sizeof(WorldMatrix), 1, fin);

            ReadString(fin, LstrMyTexturePath);
            ReadString(fin, LstrMyTexturePrototypeName);

            fread(&vStartUV, sizeof(vStartUV), 1, fin);
            fread(&vEndUV, sizeof(vEndUV), 1, fin);
            fread(&vTrigerTargetStartUV, sizeof(vStartUV), 1, fin);
            fread(&vTrigerTargetEndUV, sizeof(vEndUV), 1, fin);

            ReadString(fin, FontInfo.strFontText);
            fread(&FontInfo.vFontColor, sizeof(FontInfo.vFontColor), 1, fin);

            _vector worldPosition = XMVectorSet(WorldMatrix.m[3][0], WorldMatrix.m[3][1], WorldMatrix.m[3][2], 1.0f);
            _matrix viewProjMatrix = XMMatrixMultiply(XMMatrixIdentity(), XMMatrixOrthographicLH(1600.f, 900.f, 0.f, 1.f));
            _vector screenPosition = XMVector3TransformCoord(worldPosition, viewProjMatrix);

            _float fScreenX = XMVectorGetX(screenPosition) * 1600.f * 0.5f + 1600.f * 0.5f;
            _float fScreenY = -XMVectorGetY(screenPosition) * 900.f * 0.5f + 900.f * 0.5f;


            _vector vRight = XMLoadFloat3(&_float3(WorldMatrix.m[0][0], WorldMatrix.m[0][1], WorldMatrix.m[0][2]));
            _vector vUp = XMLoadFloat3(&_float3(WorldMatrix.m[1][0], WorldMatrix.m[1][1], WorldMatrix.m[1][2]));

            _float fSizeX = XMVectorGetX(XMVector3Length(vRight));       //  ��Ʈ x^2 + y^2 + z^2 �� �����Ǳ��̴ϱ� 
            _float fSizeY = XMVectorGetX(XMVector3Length(vUp));


            CUI::UI_DESC		Desc{};
            Desc.fX = fScreenX;
            Desc.fY = fScreenY;
            Desc.fSizeX = fSizeX;
            Desc.fSizeY = fSizeY;
            Desc.LstrTexturePrototype = LstrMyTexturePrototypeName;
            Desc.vStartUV = vStartUV;
            Desc.vEndUV = vEndUV;
            Desc.vTrigerTargetStartUV = vTrigerTargetStartUV;
            Desc.vTrigerTargetEndUV = vTrigerTargetEndUV;

            Desc.fSpeedPerSec = 10.f;
            Desc.fRotationPerSec = XMConvertToRadians(90.0f);
            Desc.pBatchObj_InitWorldMatrix = &WorldMatrix;
            Desc.FontInfo = FontInfo;

            // �����󿡼� ��¥�� ��� �̹����� �ε��ҰŶ� ��������� ����Ŭ�󿡼� �纻�������� ������ �ؽ��� ���� �������ߤ��Ѵ�
            if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, LstrLayerName,
                LstrPrototypeName, &Desc)))
                MSG_BOX(L"�ε�� ������ �ȳ���");
        }

        CUIBatch::BATCH_GROUP_DESC Desc = {};
        Desc.eGroup = (CUIBatch::BATHCING_GROUP)iBatchingGroup;
        
        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_MAPTOOL, L"Layer_UIBatch",       // �ӽ�
            TEXT("Prototype_GameObject_UIBatch") , &Desc )))
            MSG_BOX(L"�׽�Ʈ������ �ȳ���");
    }
   

    


    fclose(fin);
    MSG_BOX(TEXT("BatchUI UI �ε� ����!"));

}


_wstring CLevel_MapTool::ConvertToBinRelativePath(const _wstring& FullPath)
{
    _wstring baseDir = L"\\Bin\\";

    size_t pos = FullPath.find(baseDir);
    if (pos == _wstring::npos) {
        return L""; 
    }

    _wstring relativePath = L"../" + FullPath.substr(pos + 1); // `\` �����ϰ� `Bin`���� �����ϵ���

    return relativePath;
}

_wstring CLevel_MapTool::ConvertPathToTexturePrototypeName(const _wstring& FullPath)
{
    _wstring baseDir1 = L"\\UIAtlas\\";
    _wstring baseDir2 = L"\\UIIcon\\";
    _wstring baseDir3 = L"\\UITexture\\";
    _wstring prototypeName = L"";


    size_t pos1 = FullPath.find(baseDir1);
    size_t pos2 = FullPath.find(baseDir2);
    size_t pos3 = FullPath.find(baseDir3);

   

    size_t pos = FullPath.find(baseDir1);
    if (pos != wstring::npos) {
        pos += baseDir1.length();
        size_t nextSlash = FullPath.find(L'\\', pos);
        prototypeName = L"UIAtlas_" + FullPath.substr(pos, nextSlash - pos);
    }
    else if ((pos = FullPath.find(baseDir2)) != std::wstring::npos) {
        pos += baseDir2.length();
        size_t nextSlash = FullPath.find(L'\\', pos);
        prototypeName = L"UIIcon_" + FullPath.substr(pos, nextSlash - pos);
    }
    else if ((pos = FullPath.find(baseDir3)) != std::wstring::npos) {
        pos += baseDir3.length();
        size_t nextSlash = FullPath.find(L'\\', pos);
        prototypeName = L"UITexture_" + FullPath.substr(pos, nextSlash - pos);
    }

    prototypeName = L"Prototype_Component_Texture_" + prototypeName;
    return prototypeName;
}





CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLevel_MapTool* pInstance = new CLevel_MapTool(pDevice,pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Created : CLevel_MapTool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_MapTool::Free()
{  
    // ����
    ImGui_ImplDX11_Shutdown();  // Dx ���� 
    ImGui_ImplWin32_Shutdown(); // Win32 ���� ������� X

    ImGui::DestroyContext(); // ������ �������� �ֱ�
    // ImGui ���� �����ؾ��ؿ�~~~

   
    for (_uint i = 0; i < (_uint)UIATLAS::ATLAS_END; i++)
    {
        for (_uint j = 0; j < m_TextureUIAtlas[i].size(); j++)
        {
            Safe_Release(m_TextureUIAtlas[i][j].pTexture);
        }
        m_TextureUIAtlas[i].clear();
    }
   
    for (_uint i = 0; i < (_uint)UIICON::ICON_END; i++)
    {
        for (_uint j = 0; j < m_TextureUIIcon[i].size(); j++)
        {
            Safe_Release(m_TextureUIIcon[i][j].pTexture);
        }
        m_TextureUIIcon[i].clear();
    }
    
    for (_uint i = 0; i < (_uint)TEXTURE::TEXTURE_END; i++)
    {
        for (_uint j = 0; j < m_TextureUITexture[i].size(); j++)
        {
            Safe_Release(m_TextureUITexture[i][j].pTexture);
        }
        m_TextureUITexture[i].clear();
    }
   

    for (_uint i = 0; i < m_TextureUIMaterial.size(); i++)
    {
        Safe_Release(m_TextureUIMaterial[i].pTexture);
    }
    m_TextureUIMaterial.clear();
    


 
    Safe_Release(m_pFreeCamera);
    Safe_Release(m_pPickedUI);

    __super::Free();
}

