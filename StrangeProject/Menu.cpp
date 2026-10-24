#include "Menu.hpp"
#include "Source.hpp"
#include "Config.hpp"
#include "Player.hpp"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
ImFont*  Font = nullptr;
static char Textsz[ 64 ] = { 0 };
static bool test = true;
//extern LRESULT ImGui_ImplDX9_WndProcHandler( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
//bool angle = false;
namespace Feature
{
	Menu::Menu()
		: m_bMouse( false ),
		m_iWeaponAimbot( 0 ),
		m_iWeaponTriggerbot( 0 ),
		m_iConfig( -1 ),
		m_bAimbot( false ),
		m_bTriggerbot( false ),
		m_bESP( false ),
		m_bRender( false ),
		m_bAntiAim( false ),
		m_bRemovals( false ),
		m_bMisc( false ),
		m_bPlayers( false ),
		m_bColors( false ),
		m_bBinds( false ),
		m_bConfig( false ),
		angle( false )

	{
		ZeroMemory( m_szConfigName, sizeof( m_szConfigName ) );
	}

	Menu::~Menu()
	{
		Source::m_pCvar->FindVar( XorStr( "cl_mouseenable" ) )->m_nValue = 1;

		ImGui_ImplDX9_Shutdown();
	}

	bool Menu::Create( HWND hWnd, IDirect3DDevice9* pDevice )
	{
		ImGui::CreateContext();
		if ( !ImGui_ImplDX9_Init( hWnd, pDevice ) )
			return false;

		auto& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2( 13, 10 );
		style.WindowRounding = 4;
		//style.ChildWindowRounding = 1;
		style.FramePadding = ImVec2( 10, 2 );
		style.FrameRounding = 0;
		style.ItemSpacing = ImVec2( 9, 4 );
		style.ItemInnerSpacing = ImVec2( 8, 9 );
		style.TouchExtraPadding = ImVec2( 0, 0 );
		style.IndentSpacing = 11;
		style.ScrollbarSize = 20;
		style.ScrollbarRounding = 0;
		style.GrabMinSize = 1;
		style.GrabRounding = 0;
		//style.WindowTitleAlign = 1.5f, 1.5f;

		SetColors();

		return true;
	}

	const char* ModeList[] =
	{
		"Off",
		"Auto",
		"On Press",
	};

	const char* SpotList[] =
	{
		"Pelvis",
		"Left Thigh",
		"Left Calf",
		"Left Foot",
		"Left Toe",
		"Right Thigh",
		"Right Calf",
		"Right Foot",
		"Right Toe",
		"Spine 1",
		"Spine 2",
		"Neck",
		"Head",
		"Left Upper Arm",
		"Left Forearm",
		"Left Hand",
		"Right Upper Arm",
		"Right Forearm",
		"Right Hand",
	};

	const char* TargetSelectionList[] =
	{
		"Fast",
		"Distance",
		"Crosshair",
		"Smart",
	};

	const char* ResolverType[]=
	{
		"Off",
		"LispJitter",
		"BruteYaw",
		"Adaptive"
	};

	const char* SpamType[] =
	{
		"Off",
		"Regular Message",
		"StrangeH00K Messages",
	};

	const char* SmoothList[] =
	{
		"Off",
		"Step",
		"Linear",
	};

	const char* HitScanList[] =
	{
		"Off",
		"Medium",
		"Perfect",
	};

	const char* SilentList[] =
	{
		"Off",
		"Silent(without bSendPacket)",
		"Perfect(with bSendPacket)",
	};

	const char* AccuracyList[] =
	{
		"Normal",
		"Perfect",
		"Seed",
	};

	const char* TargetList[] =
	{
		"Everyone",
		"Enemy",
		"Friendly",
	};

	const char* BoxList[] =
	{
		"Off",
		"Normal",
		"Corners",
		"Swastika Corners",
	};

	const char* InfoTypeList[] =
	{
		"Off",
		"Text",
		"Bar",
	};

	const char* InfoAlignList[] =
	{
		"Left",
		"Right",
		"Top",
		"Bottom",
	};

	const char* ChamsModeList[] =
	{
		"Off",
		"Flat",
		"Shadow",
	};

	const char* PitchList[] =
	{
		"Off",
		"Safe Up",
		"Safe Down",
		"Fake Down",
		"Adaptive Fake(Test)",
		"Adaptive Pitch",
		"Reverse",
	};	

	const char* LagFixWithAimType[] =
	{
		"Off",
		"Type 1",
		"Type 2",
		"Disable Interp"
	};
	
	const char* FakeLagList[] =
	{
		"Off",
		"Normal",
		"LagCompensate"

	};

	const char* YawList[] =
	{
		"Off",
		"Fake BackWard",
		"Fake SideWays",
		"Fake1",
		"Jitter",
		"Dunamic Fake",
		"TwitchYaw",
	};

	const char* NoSpreadList[] =
	{
		"Off",
		"Normal",
		"Advanced",
	};

	const char* AutoStrafeList[] =
	{
		"Disable",
		"Simple",
		"Perfect",
	};

	const char* CrosshairList[] =
	{
		"Off",
		"Dot",
		"Cross",
		"Swastika",
	};

	const char* RestrictionList[] =
	{
		"Off",
		"SMAC",
		"SMAC Ultr@",
	};

	std::string GetExtension( const std::string& target )
	{
		auto found = target.find_last_of( "." );

		std::string extension( "" );

		if ( found != std::string::npos )
			extension = target.substr( found + 1, target.length() );

		return extension;
	}

	static void ShowHelpMarker( const char* desc )
	{
		ImGui::TextDisabled( "(?)" );
		if ( ImGui::IsItemHovered() )
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
			ImGui::TextUnformatted( desc );
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void AnimationMenu()
	{

	}

	void ColorStyles()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ ImGuiCol_Text ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.78f );
		style.Colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.86f, 0.93f, 0.89f, 0.78f );
		style.Colors[ ImGuiCol_WindowBg ] = ImColor( 15, 15, 15, 255 );
		style.Colors[ ImGuiCol_ChildWindowBg ] = ImColor( 15, 15, 15, 255 );
		style.Colors[ ImGuiCol_Border ] = ImColor( 50, 50, 50, 255 );
		style.Colors[ ImGuiCol_FrameBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.09f );
		style.Colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
		style.Colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.04f, 0.04f, 0.04f, 0.88f );
		style.Colors[ ImGuiCol_TitleBg ] = ImColor( 44, 44, 44, 255 );
		style.Colors[ ImGuiCol_TitleBgCollapsed ] = ImColor( 44, 44, 44, 255 );
		style.Colors[ ImGuiCol_TitleBgActive ] = ImColor( 44, 44, 44, 255 );
		style.Colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.35f, 0.35f, 0.35f, 1.00f );
		style.Colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.13f, 0.13f, 0.13f, 1.00f );
		style.Colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.09f, 0.15f, 0.16f, 1.00f );
		style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.15f, 0.60f, 0.78f, 0.78f );
		style.Colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.15f, 0.60f, 0.78f, 1.00f );
		style.Colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.15f, 0.60f, 0.78f, 1.00f );
		style.Colors[ ImGuiCol_Button ] = ImVec4( 0.24f, 0.40f, 0.95f, 1.00f );	 // 0.24f, 0.40f, 0.95f, 1.00f
		style.Colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.24f, 0.40f, 0.95f, 0.59f );
		style.Colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
		style.Colors[ ImGuiCol_Header ] = ImVec4( 0.24f, 0.40f, 0.95f, 1.00f );
		style.Colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.24f, 0.40f, 0.95f, 0.59f );
		style.Colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
		style.Colors[ ImGuiCol_ColumnHovered ] = ImVec4( 0.70f, 0.02f, 0.60f, 0.22f );
	}

	void styled()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.f;
		style.FramePadding = ImVec2( 4, 0 );
		style.WindowPadding = ImVec2( 0, 0 );
		style.ItemSpacing = ImVec2( 0, 0 );
		style.ScrollbarSize = 10.f;
		style.ScrollbarRounding = 0.f;
		style.GrabMinSize = 5.f;
		style.WindowTitleAlign = ImVec2( 0.50, 0.50 );
	}

	void ButtonColor( int r, int g, int b )
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ ImGuiCol_Button ] = ImColor( r, g, b );
		style.Colors[ ImGuiCol_ButtonHovered ] = ImColor( r, g, b );
		style.Colors[ ImGuiCol_ButtonActive ] = ImColor( r, g, b );
	}
	void Menu::OnPresentDevice()
	{

		if ( m_bMouse != Shared::m_bMenu )
		{
			m_bMouse = Shared::m_bMenu;

			Source::m_pCvar->FindVar( XorStr( "cl_mouseenable" ) )->m_nValue = ( int )!m_bMouse;
		}

		if ( !Shared::m_bMenu )
			return;


		ImGui_ImplDX9_NewFrame();
		ImGui::GetIO().MouseDrawCursor = true;

		ImGuiWindowFlags Flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

		if ( Source::m_pEngine->IsConnected() )
		{
			Source::Background( );
		}


		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		ImFontConfig font_config;
		//font_config.OversampleH = 1; //or 2 is the same
		//font_config.OversampleV = 1;
		//font_config.PixelSnapH = 1;
		//static const ImWchar ranges[] =
		//{
		//	0x0020, 0x00FF, // Basic Latin + Latin Supplement
		//	0x0400, 0x044F, // Cyrillic
		//	0,
		//};
		//io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f, &font_config, ranges );
		ImVec2 pos;
		static auto iTab = 0;
		static bool OpenDemoWindow;
		ColorStyles();
		ImGui::Begin( "StrangeProject (0.0.8)", NULL, ImVec2( 840, 560 ), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar );
		{
			pos = ImGui::GetWindowPos();

			ImGui::Columns( 2, nullptr, false );
			ImGui::SetColumnOffset( 1, 135 );

			ButtonColor( 15, 15, 15 );
			ImGui::Button( "##upprtabs", ImVec2( 114, 20 ) );

			ImGui::SameLine();
			styled();
			ButtonColor( 50, 50, 50 );
			ImGui::Button( "##rageupline", ImVec2( 1, 20 ) );

			/*rage*/
			{
				if ( iTab == 0 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##rageupline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 0 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "Aimbot", ImVec2( 118, 100 ) ) ) iTab = 0;

				ImGui::SameLine();

				if ( iTab != 0 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 0 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##ragedownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

			}

			/*legit*/
			{
				if ( iTab == 1 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##lupline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 1 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "Visuals", ImVec2( 118, 100 ) ) ) iTab = 1;

				ImGui::SameLine();

				if ( iTab != 1 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 1 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##ldownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );
			}

			/*visuals*/
			{
				if ( iTab == 2 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##vupline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 2 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "HvH", ImVec2( 118, 100 ) ) ) iTab = 2;
				ImGui::SameLine();

				if ( iTab != 2 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 2 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##vdownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );
			}

			/*misc*/
			{
				if ( iTab == 3 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##mupline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 3 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "Misc", ImVec2( 118, 100 ) ) ) iTab = 3;
				ImGui::SameLine();

				if ( iTab != 3 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 3 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##mdownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );
			}

			/*render*/
			{
				if ( iTab == 4 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##supline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 4 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "Other", ImVec2( 118, 100 ) ) ) iTab = 4;
				ImGui::SameLine();

				if ( iTab != 4 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 4 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##sdownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );
			}

			/*config*/
			{
				if ( iTab == 5 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##supline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );

				ButtonColor( 15, 15, 15 );
				if ( iTab == 5 ) style.Colors[ ImGuiCol_Text ] = ImColor( 200, 200, 200 ); else style.Colors[ ImGuiCol_Text ] = ImColor( 80, 80, 80 );
				if ( ImGui::Button( "Config", ImVec2( 118, 100 ) ) ) iTab = 5;
				ImGui::SameLine();

				if ( iTab != 5 )ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##gfgfgfgfgfgf", ImVec2( 1, 100 ) );

				if ( iTab == 5 ) ButtonColor( 50, 50, 50 ); else ButtonColor( 15, 15, 15 );
				ImGui::Button( "##sdownline", ImVec2( 118, 1 ) );

				ImGui::SameLine();

				ButtonColor( 50, 50, 50 );
				ImGui::Button( "##fgfgfg", ImVec2( 1, 1 ) );
			}

			ButtonColor( 15, 15, 15 );
			ImGui::Button( "##upprtabs", ImVec2( 118, 20 ) );

			ImGui::SameLine();

			ButtonColor( 50, 50, 50 );
			ImGui::Button( "##rageupline", ImVec2( 1, 20 ) );

			ImGui::NextColumn();
			style.WindowPadding = ImVec2( 8, 8 );
			style.ItemSpacing = ImVec2( 4, 4 );
			ColorStyles();
			ImGui::BeginChild( "main", ImVec2( 700, 540 ) );
			{
				if ( iTab == 0 )
				{
					//ImGui::SameLine();

					if ( Config::Main->AimbotWeaponConfig )
					{
						CSWeaponID i = Config::GetWeaponID( Config::WeaponList[ m_iWeaponAimbot ] );

						
					}
					else
					{
						ImGui::Checkbox( XorStr( "Aimbot" ), &Config::Main->Aimbot->Active );
						ImGui::Checkbox( XorStr( "Auto Fire" ), &Config::Main->Aimbot->AutoFire );

						ImGui::Spacing();

						ImGui::Combo( XorStr( "AimSpot" ), &Config::Main->Aimbot->Spot, SpotList, ARRAYSIZE( SpotList ) );

						ImGui::Spacing();

						ImGui::Checkbox( XorStr( "AutoWall" ), &Config::Main->Aimbot->AutoWall );
						if ( Config::Main->Aimbot->AutoWall )
							ImGui::SliderInt( XorStr( "MinDamage" ), &Config::Main->Aimbot->MinDamage, 0, 100 );

						ImGui::Spacing();

						ImGui::Combo( XorStr( "HitScan" ), &Config::Main->Aimbot->HitScan, HitScanList, ARRAYSIZE( HitScanList ) );

						ImGui::Spacing();

						//ImGui::Checkbox( XorStr( "Aim At Team" ), &Config::Main->Aimbot->AimAtTeam );
						ImGui::Combo( XorStr( "Aim Removals" ), &Config::Main->Aimbot->Silent, SilentList, ARRAYSIZE( SilentList ) );

						Config::Main->Aimbot->Clamp();
					}

				}

				if ( iTab == 1 )
				{
					ImGui::Checkbox( XorStr( "Esp Box" ), &Config::ESP->Box );

					ImGui::Spacing();

					//ImGui::Checkbox( XorStr( "Esp Name" ), &Config::ESP->Name );
					ImGui::Checkbox( XorStr( "Esp Weapon" ), &Config::ESP->Weapon );
					//ImGui::Checkbox( XorStr( "Esp Dropped Grenades" ), &Config::ESP->DroppedGrenades );
					ImGui::Checkbox( XorStr( "Esp Dropped Weapons" ), &Config::ESP->DroppedWeapons );
					ImGui::Checkbox( XorStr( "Aim Spot" ), &Config::ESP->AimSpot );

					ImGui::Checkbox( XorStr( "Esp Health" ), &Config::ESP->Health );
					ImGui::Checkbox( XorStr( "Esp Armor" ), &Config::ESP->Armor );
					ImGui::Checkbox( XorStr( "Esp Show Bots" ), &Config::ESP->ShowBots );
					ImGui::Checkbox( XorStr( "Chams" ), &Config::Render->ChamsActive );
					ImGui::SameLine();
					ImGui::Checkbox( XorStr( "Wireframe Effect" ), &Config::Render->Wireframe );
					ImGui::Checkbox( XorStr( "Hand Chams" ), &Config::Render->HandChams );
					ImGui::Checkbox( XorStr( "Resolver Marker" ), &Config::ESP->bFixPitchMarker );


					ImGui::SliderInt( "NightMode (NotWork)", &Config::Render->NightModePower, 0, 100, "%d%%" );
					ImGui::Spacing();
					ImGui::SliderInt( "Asus (NotWork)", &Config::Render->AsusWallsPower, 0, 100, "%d%%" );

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::SliderFloat( "ViewmodelFOV", &Config::ESP->ViewModelFov, 0.0f, 120.0f );
					ImGui::SliderFloat( "OverrideFOV", &Config::ESP->OverrideFOV, 0.0f, 140.0f );

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox( XorStr( "No Smoke" ), &Config::Render->NoSmoke );
					ImGui::SameLine();
					ImGui::Checkbox( XorStr( "WireframeSmoke" ), &Config::Render->WireframeSmoke );
					ImGui::Checkbox( XorStr( "No Flash" ), &Config::Render->NoFlash );
					if ( Config::Render->NoSmoke == true )
					{
						Config::Render->WireframeSmoke = false;
					}
					Config::ESP->Clamp();
					Config::Render->Clamp();
				}

				if ( iTab == 2 )
				{
					ImGui::Checkbox( XorStr( "At Target" ), &Config::AntiAim->AtTarget );
					ImGui::Checkbox( XorStr( "AA with Knife" ), &Config::AntiAim->KnifeAntiAim );
					ImGui::Checkbox( XorStr( "Enemy Check" ), &Config::AntiAim->EnemyCheck );
					ImGui::Checkbox( XorStr( "Edge Detection" ), &Config::AntiAim->EdgeDetection );
					ImGui::SameLine(); ShowHelpMarker( "Hides your head in the wall to make it harder for the enemy to hit you" );
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if ( &Config::AntiAim->AtTarget )
					{
						ImGui::SliderFloat( "Target Check Distance", &Config::AntiAim->AtTargetDistance, 0.0f, 8192.0f );
					}
					ImGui::Spacing();

					ImGui::Combo( XorStr( "Pitch" ), &Config::AntiAim->Pitch, PitchList, ARRAYSIZE( PitchList ) );
					ImGui::Combo( XorStr( "Yaw" ), &Config::AntiAim->Yaw, YawList, ARRAYSIZE( YawList ) );

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Combo( XorStr( "FakeLag" ), &Config::AntiAim->FakeLag, FakeLagList, ARRAYSIZE( FakeLagList ) );
					if ( Config::AntiAim->FakeLag > 0 )
						ImGui::SliderInt( XorStr( "Factor" ), &Config::AntiAim->ChokedPackets, 1, 14 );
					ImGui::Spacing();
						ImGui::Checkbox( XorStr( "Disable While Shooting" ), &Config::AntiAim->DisableWhileShooting );
						ImGui::SameLine(); ShowHelpMarker( "Disables FakeLag during shooting" );

					ImGui::Spacing();
					ImGui::Combo( XorStr( "LagFix with Aimbot(test)" ), &Config::AntiAim->LagFixWithAim, LagFixWithAimType, ARRAYSIZE(LagFixWithAimType));
					ImGui::Checkbox( XorStr( "Clock LagFix (test)" ), &Config::AntiAim->ClockLagFix );
					ImGui::Checkbox( XorStr( "Time Demo LagFix (test)" ), &Config::AntiAim->TimeDemoLagFix );
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					Config::AntiAim->Clamp();
				}

				if ( iTab == 3 )
				{
					ImGui::Checkbox("BunnyHop", &Config::Misc->AutoJump);
					ImGui::Checkbox("AutoPistol", &Config::Misc->AutoPistol );
					ImGui::Combo( XorStr( "Strafe Helper" ), &Config::Misc->AutoStrafe, AutoStrafeList, ARRAYSIZE( AutoStrafeList ) );
					ImGui::Checkbox( "Circle Strafe", &Config::Misc->CircleStrafe );
					ImGui::PushItemWidth( 170 );
					ImGui::KeyButton( XorStr( "CircleStrafeKey" ), &Config::Misc->CircleStrafeKey );
					ImGui::PopItemWidth();
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Checkbox( "No Spread", &Config::Removals->NoSpread );
					ImGui::Checkbox( XorStr( "No Recoil" ), &Config::Removals->NoRecoil );
					ImGui::Checkbox( XorStr( "No Visual Recoil" ), &Config::Removals->NoVisualRecoil );

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox( "ThirdPerson", &Config::Misc->ThirdPerson );

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Combo( XorStr( "Resolver" ), &Config::Misc->Resolver, ResolverType, ARRAYSIZE( ResolverType ) );
					ImGui::Checkbox( "AutoFixPitch", &Config::Misc->AutoFixPitch );
					if ( Config::Misc->Resolver == 1 || Config::Misc->Resolver == 2 )
					{
						ImGui::SliderInt( "ShotsFired", &Config::Misc->ShotsFired, 1, 10 );
						ImGui::SliderFloat( "CorrectioSpeedPower", &Config::Misc->CorrectioSpeedPower, 20, 200 );
						ImGui::SameLine(); ShowHelpMarker( "The force with which the enemy will spin" );
					}

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Combo( XorStr( "Spam" ), &Config::Misc->SpamInChat, SpamType, ARRAYSIZE( SpamType ) );
					if ( Config::Misc->SpamInChat == 1 )
					{
						ImGui::InputText( "Regular Message", Config::Misc->RegularMessage, sizeof( Config::Misc->RegularMessage ) );
						ImGui::SliderInt( "Spam Time", &Config::Misc->SpamTime, 5, 40 );
					}
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Checkbox( "HitSound", &Config::Misc->HitSound );
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::Checkbox( "RoundSay", &Config::Misc->RoundSay );
					ImGui::SameLine(); ShowHelpMarker( "Do not use it with normal spam" );
					if ( Config::Misc->RoundSay )
					{
						ImGui::Spacing();
						ImGui::InputText( "RoundStartSay", Config::Misc->RoundStartSay, sizeof( Config::Misc->RoundStartSay ) );
						ImGui::Spacing();
						ImGui::InputText( "RoundEndSay", Config::Misc->RoundEndSay, sizeof( Config::Misc->RoundEndSay ) );
					}

					if ( Config::Misc->SpamInChat > 0 && Config::Misc->RoundSay == true )
					{
						ImGui::OpenPopup( XorStr( "Information" ) );
						if ( ImGui::BeginPopupModal( XorStr( "Information" ), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
						{
							ImGui::Text( "If spam is enabled, roundsay will not work!" );
							ImGui::Spacing();

							if ( ImGui::Button( XorStr( "OK, I understand" ), ImVec2( 325, 0 ) ) )
							{
								Config::Misc->RoundSay = false;
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}
					}
					Config::Misc->Clamp();
				}

				if ( iTab == 4 )
				{
					ImGui::ListBoxHeader( "" );

					WIN32_FIND_DATAA ffd;
					LARGE_INTEGER filesize;
					CHAR szDir[ MAX_PATH ];
					HANDLE hFind = INVALID_HANDLE_VALUE;

					std::string strPath = Config::GetPath();

					strncpy_s( szDir, strPath.c_str(), MAX_PATH );
					strncat_s( szDir, "*", MAX_PATH );

					hFind = FindFirstFileA( szDir, &ffd );

					std::vector<std::string> files;

					int index = 0;

					if ( hFind != INVALID_HANDLE_VALUE )
					{
						do
						{
							auto ext = GetExtension( ffd.cFileName );

							if ( !( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && ext.compare( "ini" ) == 0 )
							{
								const bool item_selected = ( index == m_iConfig );
								filesize.LowPart = ffd.nFileSizeLow;
								filesize.HighPart = ffd.nFileSizeHigh;
								char szText[ MAX_PATH ];
								sprintf_s( szText, sizeof( szText ), "%s [%lld bytes]", ffd.cFileName, filesize.QuadPart );
								ImGui::PushID( index );
								if ( ImGui::Selectable( szText, item_selected ) )
								{
									m_iConfig = index;
									strncpy_s( m_szConfigName, ffd.cFileName, sizeof( m_szConfigName ) );
								}
								ImGui::PopID();
								index++;
								files.push_back( ffd.cFileName );
							}
						}
						while ( FindNextFileA( hFind, &ffd ) != 0 );

						FindClose( hFind );
					}

					ImGui::ListBoxFooter();

					ImGui::InputText( "", m_szConfigName, sizeof( m_szConfigName ) );

					if ( ImGui::Button( XorStr( "Load" ) ) && m_iConfig != -1 )
					{
						Shared::m_strConfig = files[ m_iConfig ];
						Shared::m_bLoad = true;
					}

					ImGui::SameLine();

					if ( ImGui::Button( XorStr( "Save" ) ) && !std::string( m_szConfigName ).empty() )
					{
						Shared::m_strConfig = m_szConfigName;
						Shared::m_bSave = true;
					}

					ImGui::SameLine();

					if ( ImGui::Button( XorStr( "Remove" ) ) )
						ImGui::OpenPopup( XorStr( "Delete?" ) );
					if ( ImGui::BeginPopupModal( XorStr( "Delete?" ), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
					{
						ImGui::Text( "Are you sure?", m_szConfigName );
						ImGui::Spacing();

						if ( ImGui::Button( XorStr( "Yes" ), ImVec2( 125, 0 ) ) )
						{
							Config::Delete( m_szConfigName );
							ImGui::CloseCurrentPopup();
						}

						ImGui::SameLine();

						if ( ImGui::Button( XorStr( "No" ), ImVec2( 125, 0 ) ) )
							ImGui::CloseCurrentPopup();

						ImGui::EndPopup();
					}
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::PushItemWidth( 230 );
					ImGui::SliderInt("BackGround Darkness", &Config::Misc->BackGroundDarkness, 0, 100, "%d%%");
				}

				if ( m_bPlayers )
				{
					ImGui::SetNextWindowSize( { 500, 400 }, ImGuiSetCond_Appearing );
					ImGui::Begin( XorStr( "Players" ), nullptr, ImGuiWindowFlags_NoCollapse );
					{
						RenderPlayers();
					}
					ImGui::End();
				}

				if ( m_bColors )
				{
					ImGui::Begin( XorStr( "Colors" ), nullptr, Flags );
					{
						ImGui::Text( XorStr( "ESP" ) );
						ImGui::BeginChild( XorStr( "ESP" ), ImVec2( 280, 110 ), true );
						{
							ImGui::ColorEdit4( XorStr( "T Normal" ), m_flColors[ COL_T_ESP_NORMAL ] );
							ImGui::ColorEdit4( XorStr( "T Colored" ), m_flColors[ COL_T_ESP_COLORED ] );
							ImGui::ColorEdit4( XorStr( "CT Normal" ), m_flColors[ COL_CT_ESP_NORMAL ] );
							ImGui::ColorEdit4( XorStr( "CT Colored" ), m_flColors[ COL_CT_ESP_COLORED ] );
						}
						ImGui::EndChild();

						ImGui::Text( XorStr( "Chams" ) );
						ImGui::BeginChild( XorStr( "Chams" ), ImVec2( 280, 110 ), true );
						{
							ImGui::ColorEdit4( XorStr( "T Normal" ), m_flColors[ COL_T_CHAMS_NORMAL ] );
							ImGui::ColorEdit4( XorStr( "T Colored" ), m_flColors[ COL_T_CHAMS_COLORED ] );
							ImGui::ColorEdit4( XorStr( "CT Normal" ), m_flColors[ COL_CT_CHAMS_NORMAL ] );
							ImGui::ColorEdit4( XorStr( "CT Colored" ), m_flColors[ COL_CT_CHAMS_COLORED ] );
						}
						ImGui::EndChild();

						ImGui::Text( XorStr( "Other" ) );
						ImGui::BeginChild( XorStr( "Other" ), ImVec2( 280, 90 ), true );
						{
							ImGui::ColorEdit3( XorStr( "Main" ), m_flColors[ COL_MAIN ] );
							ImGui::ColorEdit4( XorStr( "Crosshair" ), m_flColors[ COL_CROSSHAIR ] );
						}
						ImGui::EndChild();

						if ( ImGui::Button( XorStr( "Apply" ) ) )
							ApplyColors();

						ImGui::SameLine();

						if ( ImGui::Button( XorStr( "Reset" ) ) )
							ResetColors();
					}
					ImGui::End(); // 
				}

				if ( m_bBinds )
				{
					ImGui::Begin( XorStr( "Binds" ), nullptr, Flags );
					{
						ImGui::InputInt( XorStr( "Menu" ), &Config::Binds->Menu );
						ImGui::InputInt( XorStr( "Eject" ), &Config::Binds->Eject );
						ImGui::InputInt( XorStr( "Panic" ), &Config::Binds->Panic );
					}
					ImGui::End();
				}

				if ( iTab == 5 )
				{
					
				}
			}ImGui::EndChild();

		}
		ImGui::End();

		style.Colors[ ImGuiCol_WindowBg ] = ImColor( 30, 30, 30, 255 );
		ImGui::SetNextWindowPos( ImVec2( pos.x - 4, pos.y - 4 ) );
		ImGui::Begin( "StrangeProjectOutLine", NULL, ImVec2( 848, 568 ), 1.f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus );
		{

		}ImGui::End();



		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
	}

	bool Menu::OnKeyEvent( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		if ( !Shared::m_bMenu )
			return false;

		if ( ImGui_ImplDX9_WndProcHandler( hWnd, message, wParam, lParam ) )
			return true;

		return false;
	}

	void Menu::OnLostDevice()
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	void Menu::OnResetDevice()
	{
		ImGui_ImplDX9_CreateDeviceObjects();
	}

	const char* TeamList[] =
	{
		"None",
		"Spectator",
		"T",
		"CT",
	};

	const char* PitchModList[] =
	{
		"Normal",
		"Zero",
		"Up",
		"Down",
		"Resolver",
	};

	const char* YawModList[] =
	{
		"Normal",
		"Zero",
		"Reversed",
		"Forward",
		"Backward",
		"Sideway Left",
		"Sideway Right",
	};

	void Menu::RenderPlayers()
	{
		ImGui::Columns( 6, "players_columns" );
		ImGui::Spacing();
		ImGui::Text( XorStr( "Name" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Team" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "SteamID" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "UserID" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Pitch" ) ); ImGui::NextColumn();
		ImGui::Text( XorStr( "Yaw" ) ); ImGui::NextColumn();
		ImGui::Spacing();

		auto local = C_CSPlayer::GetLocalPlayer();

		if ( local )
		{
			int size = Source::m_pEngine->GetMaxClients();

			for ( int i = 0; i <= size; i++ )
			{
				auto player = ToCSPlayer( Source::m_pEntList->GetBaseEntity( i ) );

				if ( !player )
					continue;

				auto player_from_list = Source::m_pPlayerList->GetPlayer( i );

				if ( !player_from_list )
					continue;

				int team = player->m_iTeamNum();

				if ( team > 3 )
					team = 0;

				player_info_t data;
				if ( !Source::m_pEngine->GetPlayerInfo( i, &data ) )
					continue;


				ImGui::Text( data.name ); ImGui::NextColumn();
				ImGui::Text( TeamList[ team ] ); ImGui::NextColumn();

				char guid[ 32 ];
				sprintf_s( guid, sizeof( guid ), "##guid_%i", i );

				ImGui::InputText( guid, data.guid, sizeof( data.guid ), ImGuiInputTextFlags_ReadOnly ); ImGui::NextColumn();
				ImGui::Text( std::to_string( data.userID ).c_str() ); ImGui::NextColumn();

				char pitch[ 32 ];
				sprintf_s( pitch, sizeof( pitch ), "##pitch_%i", i );

				char yaw[ 32 ];
				sprintf_s( yaw, sizeof( yaw ), "##yaw_%i", i );

				ImGui::Combo( pitch, &player_from_list->m_pitch, PitchModList, ARRAYSIZE( PitchModList ) ); ImGui::NextColumn();
				ImGui::Combo( yaw, &player_from_list->m_yaw, YawModList, ARRAYSIZE( YawModList ) ); ImGui::NextColumn();
			}
		}

		ImGui::Columns( 1 );
		ImGui::Spacing();
	}

	void Menu::SetColors()
	{
		for ( int i = 0; i < 4; i++ )
		{
			m_flColors[ COL_T_ESP_NORMAL ][ i ] = Config::Colors->T_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_ESP_COLORED ][ i ] = Config::Colors->T_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_NORMAL ][ i ] = Config::Colors->T_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_COLORED ][ i ] = Config::Colors->T_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CT_ESP_NORMAL ][ i ] = Config::Colors->CT_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_ESP_COLORED ][ i ] = Config::Colors->CT_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_NORMAL ][ i ] = Config::Colors->CT_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_COLORED ][ i ] = Config::Colors->CT_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CROSSHAIR ][ i ] = Config::Colors->Crosshair[ i ] / 255.0f;
			m_flColors[ COL_MAIN ][ i ] = Config::Colors->Main[ i ] / 255.0f;
		}
	}

	void Menu::ApplyColors()
	{
		for ( int i = 0; i < 4; i++ )
		{
			Config::Colors->T_ESP_Normal[ i ] = ( int )( m_flColors[ COL_T_ESP_NORMAL ][ i ] * 255.0f );
			Config::Colors->T_ESP_Colored[ i ] = ( int )( m_flColors[ COL_T_ESP_COLORED ][ i ] * 255.0f );
			Config::Colors->T_Chams_Normal[ i ] = ( int )( m_flColors[ COL_T_CHAMS_NORMAL ][ i ] * 255.0f );
			Config::Colors->T_Chams_Colored[ i ] = ( int )( m_flColors[ COL_T_CHAMS_COLORED ][ i ] * 255.0f );

			Config::Colors->CT_ESP_Normal[ i ] = ( int )( m_flColors[ COL_CT_ESP_NORMAL ][ i ] * 255.0f );
			Config::Colors->CT_ESP_Colored[ i ] = ( int )( m_flColors[ COL_CT_ESP_COLORED ][ i ] * 255.0f );
			Config::Colors->CT_Chams_Normal[ i ] = ( int )( m_flColors[ COL_CT_CHAMS_NORMAL ][ i ] * 255.0f );
			Config::Colors->CT_Chams_Colored[ i ] = ( int )( m_flColors[ COL_CT_CHAMS_COLORED ][ i ] * 255.0f );

			Config::Colors->Crosshair[ i ] = ( int )( m_flColors[ COL_CROSSHAIR ][ i ] * 255.0f );
			Config::Colors->Main[ i ] = ( int )( m_flColors[ COL_MAIN ][ i ] * 255.0f );
		}

		for ( int i = 0; i < 3; i++ )
		{
			ImGuiStyle& Style = ImGui::GetStyle();

			//Style.Colors[ ImGuiCol_ScrollbarGrab ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_ScrollbarGrabHovered ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_ScrollbarGrabActive ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_CheckMark ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_SliderGrab ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_SliderGrabActive ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_Button ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_ButtonHovered ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_ButtonActive ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_Header ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_HeaderHovered ][ i ] = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_HeaderActive ][ i ] = m_flColors[ COL_MAIN ][ i ];
			////Style.Colors[ ImGuiCol_CloseButton ][ i ]           = m_flColors[ COL_MAIN ][ i ];
			////Style.Colors[ ImGuiCol_CloseButtonHovered ][ i ]    = m_flColors[ COL_MAIN ][ i ];
			////Style.Colors[ ImGuiCol_CloseButtonActive ][ i ]     = m_flColors[ COL_MAIN ][ i ];
			//Style.Colors[ ImGuiCol_TextSelectedBg ][ i ] = m_flColors[ COL_MAIN ][ i ];
		}
	}

	void Menu::ResetColors()
	{
		for ( int i = 0; i < 4; i++ )
		{
			m_flColors[ COL_T_ESP_NORMAL ][ i ] = Config::Colors->T_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_ESP_COLORED ][ i ] = Config::Colors->T_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_NORMAL ][ i ] = Config::Colors->T_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_T_CHAMS_COLORED ][ i ] = Config::Colors->T_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CT_ESP_NORMAL ][ i ] = Config::Colors->CT_ESP_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_ESP_COLORED ][ i ] = Config::Colors->CT_ESP_Colored[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_NORMAL ][ i ] = Config::Colors->CT_Chams_Normal[ i ] / 255.0f;
			m_flColors[ COL_CT_CHAMS_COLORED ][ i ] = Config::Colors->CT_Chams_Colored[ i ] / 255.0f;

			m_flColors[ COL_CROSSHAIR ][ i ] = Config::Colors->Crosshair[ i ] / 255.0f;
			m_flColors[ COL_MAIN ][ i ] = Config::Colors->Main[ i ] / 255.0f;
		}

		for ( int i = 0; i < 3; i++ )
		{
			ImGuiStyle& Style = ImGui::GetStyle();

			//Style.Colors[ ImGuiCol_ScrollbarGrab ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_ScrollbarGrabHovered ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_ScrollbarGrabActive ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_CheckMark ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_SliderGrab ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_SliderGrabActive ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_Button ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_ButtonHovered ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_ButtonActive ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_Header ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_HeaderHovered ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_HeaderActive ][ i ] = Config::Colors->Main[ i ] / 255.0f;
			////Style.Colors[ ImGuiCol_CloseButton ][ i ]           = Config::Colors->Main[ i ] / 255.0f;
			////Style.Colors[ ImGuiCol_CloseButtonHovered ][ i ]    = Config::Colors->Main[ i ] / 255.0f;
			////Style.Colors[ ImGuiCol_CloseButtonActive ][ i ]     = Config::Colors->Main[ i ] / 255.0f;
			//Style.Colors[ ImGuiCol_TextSelectedBg ][ i ] = Config::Colors->Main[ i ] / 255.0f;
		}
	}
}