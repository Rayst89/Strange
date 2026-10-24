#include "Config.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Source.hpp"
#include "Menu.hpp"

#include <iostream>
#include <fstream>

namespace Config
{
	MainList*		Main = nullptr;
	CurrentList*	Current = nullptr;
	CurrentList*	Weapon[ WEAPON_MAX ];

	ESPList*		ESP = nullptr;
	RenderList*		Render = nullptr;
	AntiAimList*	AntiAim = nullptr;
	RemovalsList*	Removals = nullptr;
	MiscList*		Misc = nullptr;
	ColorsList*		Colors = nullptr;
	BindsList*		Binds = nullptr;

	std::string		m_config;
	std::string		m_current;

	void Startup( HMODULE hMod )
	{
		Main = new MainList();
		Main->Aimbot = new AimbotList();
		Main->Triggerbot = new TriggerbotList();

		Current = new CurrentList();
		Current->Aimbot = new AimbotList();
		Current->Triggerbot = new TriggerbotList();

		for( int i = 0; i < WEAPON_MAX; i++ )
		{
			Weapon[ i ] = new CurrentList();

			Weapon[ i ]->Aimbot = new AimbotList();
			Weapon[ i ]->Triggerbot = new TriggerbotList();
		}
	
		ESP = new ESPList();
		Render = new RenderList();
		AntiAim = new AntiAimList();
		Removals = new RemovalsList();
		Misc = new MiscList();
		Colors = new ColorsList();
		Binds = new BindsList();

		m_config.append( XorStr( "\\StrangeDirectory\\" ) );

		Shared::m_strConfig = "defaultcfg";
		Shared::m_bLoad = true;
	}

	void Release()
	{
		Memory::SafeDelete( Main->Aimbot );
		Memory::SafeDelete( Main );

		Memory::SafeDelete( Current->Aimbot );
		Memory::SafeDelete( Current );

		for( int i = 0; i < WEAPON_MAX; i++ )
		{
			Memory::SafeDelete( Weapon[ i ]->Aimbot );

			Memory::SafeDelete( Weapon[ i ] );
		}

		Memory::SafeDelete( ESP );
		Memory::SafeDelete( Render );
		Memory::SafeDelete( AntiAim );
		Memory::SafeDelete( Removals );
		Memory::SafeDelete( Misc );
		Memory::SafeDelete( Colors );
		Memory::SafeDelete( Binds );
	}

	bool LoadBool( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );

		if( strcmp( szData, XorStr( "on" ) ) == 0 || strcmp( szData, XorStr( "true" ) ) == 0 || atoi( szData ) == 1 )
			return true;

		return false;
	}

	int LoadInt( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return atoi( szData );
	}

	float LoadFloat( const std::string& strSection, const std::string& strName )
	{
		char szData[ MAX_PATH ];
		GetPrivateProfileString( strSection.c_str(), strName.c_str(), XorStr( "0" ), szData, MAX_PATH, m_current.c_str() );
		return ( float )atof( szData );
	}

	std::string LoadCustomString( const std::string& app_name, const std::string& name )
	{
		char data[ 260 ] = { };
		GetPrivateProfileString( app_name.c_str(), name.c_str(), "", data, 260, m_current.c_str() );
		return data;
	}

	Color LoadColor( const std::string& strSection, const std::string& strName )
	{
		Color result;
		result.R = LoadInt( strSection, std::string( strName ).append( ".r" ) );
		result.G = LoadInt( strSection, std::string( strName ).append( ".g" ) );
		result.B = LoadInt( strSection, std::string( strName ).append( ".b" ) );
		result.A = LoadInt( strSection, std::string( strName ).append( ".a" ) );
		return result;
	}

	void SaveBool( const std::string& strSection, const std::string& strName, bool bValue )
	{
		const char* szData = bValue ? XorStr( "true" ) : XorStr( "false" );
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), szData, m_current.c_str() );
	}

	void SaveInt( const std::string& strSection, const std::string& strName, int iValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( iValue ).c_str(), m_current.c_str() );
	}

	void SaveFloat( const std::string& strSection, const std::string& strName, float flValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), std::to_string( flValue ).c_str(), m_current.c_str() );
	}	
	
	void SaveString( const std::string& strSection, const std::string& strName, const std::string& stValue )
	{
		WritePrivateProfileString( strSection.c_str(), strName.c_str(), stValue.c_str(), m_current.c_str() );
	}

	void SaveColor( const std::string& strSection, const std::string& strName, const Color& color )
	{
		SaveInt( strSection, std::string( strName ).append( ".r" ), color.R );
		SaveInt( strSection, std::string( strName ).append( ".g" ), color.G );
		SaveInt( strSection, std::string( strName ).append( ".b" ), color.B );
		SaveInt( strSection, std::string( strName ).append( ".a" ), color.A );
	}

	bool Exists( const std::string& name )
	{
		std::ifstream f( name );
		return f.good();
	}

	bool Create( const std::string& name )
	{
		auto dir = name.substr( 0, name.find_last_of( "\\" ) );

		if( CreateDirectory( dir.c_str(), nullptr ) || GetLastError() == ERROR_ALREADY_EXISTS )
		{
			std::ofstream f( name );

			if( f.good() )
			{
				f.close();

				return true;
			}
		}

		return false;
	}

	void Load( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".ini" ) ) == std::string::npos )
			current.append( XorStr( ".ini" ) );

		m_current = m_config + current;

		if( !Exists( m_current ) )
			Save( name );

		std::string main( XorStr( "-StrangeProject-" ) );


		Main->Aimbot->Active			= LoadBool( main, XorStr( "Aimbot->Active" ) );
		Main->Aimbot->AutoFire			= LoadBool( main, XorStr( "Aimbot->AutoFire" ) );
		Main->Aimbot->Spot				= LoadInt( main, XorStr( "Aimbot->AimSpot" ) );
		Main->Aimbot->AutoWall			= LoadBool( main, XorStr( "Aimbot->AutoWall" ) );
		Main->Aimbot->MinDamage			= LoadInt( main, XorStr( "Aimbot->MinDamage" ) );
		Main->Aimbot->HitScan			= LoadInt( main, XorStr( "Aimbot->HitScan" ) );
		Main->Aimbot->Silent			= LoadInt( main, XorStr( "Aimbot->Removals" ) );
		Main->Aimbot->AimAtTeam			= LoadBool( main, XorStr( "Aimbot->AimAtTeam" ) );

		Main->Aimbot->Clamp();

		ESP->Box						= LoadBool( main, XorStr( "ESP->Box" ) );
		ESP->Weapon						= LoadBool( main, XorStr( "ESP->Weapon" ) );
		ESP->DroppedWeapons				= LoadBool( main, XorStr( "ESP->DroppedWeapons" ) );
		ESP->AimSpot					= LoadBool( main, XorStr( "ESP->AimSpot" ) );
		ESP->Health						= LoadBool( main, XorStr( "ESP->Health" ) );
		ESP->Armor						= LoadBool( main, XorStr( "ESP->Armor" ) );
		ESP->Skeleton					= LoadBool( main, XorStr( "ESP->Skeleton" ) );
		ESP->ShowBots					= LoadBool( main, XorStr( "ESP->ShowBots" ) );
		Render->ChamsActive				= LoadBool( main, XorStr( "ESP->Chams" ) );
		Render->Wireframe				= LoadBool( main, XorStr( "ESP->Chams->WireframeEffect" ) );
		Render->HandChams				= LoadBool( main, XorStr( "ESP->HandChams" ) );
		ESP->ViewModelFov				= LoadFloat( main, XorStr( "ESP->ViewModelFov" ) );
		ESP->OverrideFOV				= LoadFloat( main, XorStr( "ESP->OverrideFOV" ) );
		ESP->bFixPitchMarker			= LoadBool( main, XorStr( "ESP->FixPitchMarker" ) );
		Render->NightModePower			= LoadInt( main, XorStr( "ESP->NightModePower" ) );
		Render->AsusWallsPower			= LoadInt( main, XorStr( "ESP->AsusWallsPower" ) );
		Render->NoSmoke					= LoadBool( main, XorStr( "ESP->NoSmoke" ) );
		Render->NoFlash					= LoadBool( main, XorStr( "ESP->NoFlash" ) );
		Render->WireframeSmoke			= LoadBool( main, XorStr( "ESP->WireframeSmoke" ) );
		ESP->Clamp();

		AntiAim->AtTarget				= LoadBool( main, XorStr( "HvH->AtTarget" ) );
		AntiAim->KnifeAntiAim			= LoadBool( main, XorStr( "HvH->AAWithKnife" ) );
		AntiAim->EnemyCheck				= LoadBool( main, XorStr( "HvH->EnemyCheck" ) );
		AntiAim->EdgeDetection			= LoadBool( main, XorStr( "HvH->EdgeDetection" ) );
		AntiAim->bFakeDuck				= LoadBool( main, XorStr( "HvH->FakeDuck" ) );
		AntiAim->Pitch					= LoadInt( main, XorStr( "HvH->Pitch" ) );
		AntiAim->Yaw					= LoadInt( main, XorStr( "HvH->Yaw" ) );
		AntiAim->AtTargetDistance		= LoadInt( main, XorStr( "HvH->AtTarget->AtTargetDistance" ) );
		AntiAim->FakeLag				= LoadInt( main, XorStr( "HvH->FakeLag" ) );
		AntiAim->ChokedPackets			= LoadInt( main, XorStr( "HvH->FakeLag->Factor" ) );
		AntiAim->DisableWhileShooting	= LoadBool( main, XorStr( "HvH->FakeLag->DisableWhileShooting" ) );
		AntiAim->LagFixWithAim			= LoadInt( main, XorStr( "HvH->LagFix->LagFixWithAim" ) );
		AntiAim->ClockLagFix			= LoadBool( main, XorStr( "HvH->LagFix->ClockLagFix" ) );
		AntiAim->TimeDemoLagFix			= LoadBool( main, XorStr( "HvH->LagFix->TimeDemoLagFix" ) );

		AntiAim->Clamp();

		Misc->AutoJump					= LoadBool( main, XorStr( "Misc->AutoJump" ) );
		Misc->AutoPistol				= LoadBool( main, XorStr( "Misc->AutoPistol" ) );
		Misc->AutoStrafe				= LoadInt( main, XorStr( "Misc->AutoStrafe" ) );
		Misc->CircleStrafe				= LoadBool( main, XorStr( "Misc->CircleStrafe" ) );
		Misc->CircleStrafeKey			= LoadInt( main, XorStr( "Misc->CircleStrafe->CircleStrafeKey" ) );
		Removals->NoSpread 				= LoadBool( main, XorStr( "Misc->NoSpread" ) );
		Removals->NoRecoil				= LoadBool( main, XorStr( "Misc->NoRecoil" ) );
		Removals->NoVisualRecoil		= LoadBool( main, XorStr( "Misc->NoVisualRecoil" ) );
		Misc->ThirdPerson				= LoadBool( main, XorStr( "Misc->ThirdPerson" ) );
		Misc->Resolver					= LoadInt( main, XorStr( "Misc->Resolver" ) );
		Misc->AutoFixPitch				= LoadBool( main, XorStr( "Misc->Resolver->AutoFixPitch" ) );
		Misc->ShotsFired				= LoadInt( main, XorStr( "Misc->Resolver->ShotsFired" ) );
		Misc->CorrectioSpeedPower		= LoadFloat( main, XorStr( "Misc->Resolver->CorrectioSpeedPower" ) );
		Misc->SpamInChat				= LoadInt( main, XorStr( "Misc->Spam->SpamInChatType" ) );
		Misc->SpamTime					= LoadInt( main, XorStr( "Misc->Spam->SpamTime" ) );
		auto RegularMessage				= LoadCustomString( main, "Misc->Spam->RegularMessage" );
		sprintf_s( Misc->RegularMessage, "%s", RegularMessage.data() );
		Misc->RoundSay					= LoadBool( main, XorStr( "Misc->RoundSay" ) );
		auto RoundStartSayMessage		= LoadCustomString( main, "Misc->RoundSay->RoundStartSay" );
		sprintf_s( Misc->RoundStartSay, "%s", RoundStartSayMessage.data() );		
		auto RoundEndSayMessage		= LoadCustomString( main, "Misc->RoundSay->RoundEndSay" );
		sprintf_s( Misc->RoundEndSay, "%s", RoundEndSayMessage.data() );

		Misc->HitSound					= LoadBool( main, XorStr( "Misc->HitSound" ) );
		Misc->BackGroundDarkness		= LoadInt( main, XorStr( "Misc->Menu->BackGroundDarkness" ) );
		Misc->Clamp();

		/*std::string colors( XorStr( "colors" ) );*/

		//Colors->T_ESP_Normal			= LoadColor( colors, XorStr( "t.esp.normal" ) );
		//Colors->T_ESP_Colored			= LoadColor( colors, XorStr( "t.esp.colored" ) );
		//Colors->T_Chams_Normal			= LoadColor( colors, XorStr( "t.chams.normal" ) );
		//Colors->T_Chams_Colored			= LoadColor( colors, XorStr( "t.chams.colored" ) );
		//Colors->CT_ESP_Normal			= LoadColor( colors, XorStr( "ct.esp.normal" ) );
		//Colors->CT_ESP_Colored			= LoadColor( colors, XorStr( "ct.esp.colored" ) );
		//Colors->CT_Chams_Normal			= LoadColor( colors, XorStr( "ct.chams.normal" ) );
		//Colors->CT_Chams_Colored		= LoadColor( colors, XorStr( "ct.chams.colored" ) );
		//Colors->Crosshair				= LoadColor( colors, XorStr( "crosshair" ) );
		//Colors->Main					= LoadColor( colors, XorStr( "main" ) );

		Source::m_pMenu->SetColors();
		Source::m_pMenu->ApplyColors();

		std::string binds( XorStr( "binds" ) );
	}

	void Save( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".ini" ) ) == std::string::npos )
			current.append( XorStr( ".ini" ) );

		m_current = m_config + current;

		if( !Exists( m_current ) )
		{
			if( !Create( m_current ) )
				printf_s( XorStr( "[Config::Save] Can't create config directory or file!" ) );
		}

		std::string main( XorStr( "-StrangeProject-" ) );

		Main->Aimbot->Clamp();
		Misc->Clamp();
		SaveBool( main, XorStr( "Aimbot->Active" ), Main->Aimbot->Active );
		SaveBool( main, XorStr( "Aimbot->AutoFire" ), Main->Aimbot->AutoFire );
		SaveInt( main, XorStr( "Aimbot->AimSpot" ), Main->Aimbot->Spot );
		SaveBool( main, XorStr( "Aimbot->AutoWall" ), Main->Aimbot->AutoWall );
		SaveInt( main, XorStr( "Aimbot->MinDamage" ), Main->Aimbot->MinDamage );
		SaveInt( main, XorStr( "Aimbot->HitScan" ), Main->Aimbot->HitScan );
		SaveInt( main, XorStr( "Aimbot->Removals" ), Main->Aimbot->Silent );
		SaveBool( main, XorStr( "Aimbot->AimAtTeam" ), Main->Aimbot->AimAtTeam );

		SaveBool( main, XorStr( "ESP->Box" ), ESP->Box );
		SaveBool( main, XorStr( "ESP->Weapon" ), ESP->Weapon );
		SaveBool( main, XorStr( "ESP->DroppedWeapons" ), ESP->DroppedWeapons );
		SaveBool( main, XorStr( "ESP->AimSpot" ), ESP->AimSpot );
		SaveBool( main, XorStr( "ESP->Health" ), ESP->Health );
		SaveBool( main, XorStr( "ESP->Armor" ), ESP->Armor );
		SaveBool( main, XorStr( "ESP->Skeleton" ), ESP->Skeleton );
		SaveBool( main, XorStr( "ESP->ShowBots" ), ESP->ShowBots );
		SaveBool( main, XorStr( "ESP->Chams" ), Render->ChamsActive );
		SaveBool( main, XorStr( "ESP->Chams->WireframeEffect" ), Render->Wireframe );
		SaveBool( main, XorStr( "ESP->HandChams" ), Render->HandChams );
		SaveFloat( main, XorStr( "ESP->ViewModelFov" ), ESP->ViewModelFov );
		SaveFloat( main, XorStr( "ESP->OverrideFOV" ), ESP->OverrideFOV );
		SaveBool( main, XorStr( "ESP->FixPitchMarker" ), ESP->bFixPitchMarker );
		SaveInt( main, XorStr( "ESP->NightModePower" ), Render->NightModePower );
		SaveInt( main, XorStr( "ESP->AsusWallsPower" ), Render->AsusWallsPower );
		SaveBool( main, XorStr( "ESP->NoSmoke" ), Render->NoSmoke );
		SaveBool( main, XorStr( "ESP->NoFlash" ), Render->NoFlash );
		SaveBool( main, XorStr( "ESP->WireframeSmoke" ), Render->WireframeSmoke );

		AntiAim->Clamp();

		SaveBool( main, XorStr( "HvH->AtTarget" ), AntiAim->AtTarget );
		SaveBool( main, XorStr( "HvH->AAWithKnife" ), AntiAim->KnifeAntiAim );
		SaveBool( main, XorStr( "HvH->EnemyCheck" ), AntiAim->EnemyCheck );
		SaveBool( main, XorStr( "HvH->EdgeDetection" ), AntiAim->EdgeDetection );
		SaveBool( main, XorStr( "HvH->FakeDuck" ), AntiAim->bFakeDuck );
		SaveInt( main, XorStr( "HvH->Pitch" ), AntiAim->Pitch );
		SaveInt( main, XorStr( "HvH->Yaw" ), AntiAim->Yaw );
		SaveFloat( main, XorStr( "HvH->AtTarget->AtTargetDistance" ), AntiAim->AtTargetDistance );
		SaveInt( main, XorStr( "HvH->FakeLag" ), AntiAim->FakeLag );
		SaveInt( main, XorStr( "HvH->FakeLag->Factor" ), AntiAim->ChokedPackets );
		SaveBool( main, XorStr( "HvH->FakeLag->DisableWhileShooting" ), AntiAim->DisableWhileShooting );
		SaveInt( main, XorStr( "HvH->LagFix->LagFixWithAim" ), AntiAim->LagFixWithAim );
		SaveBool( main, XorStr( "HvH->LagFix->ClockLagFix" ), AntiAim->ClockLagFix );
		SaveBool( main, XorStr( "HvH->LagFix->TimeDemoLagFix" ), AntiAim->TimeDemoLagFix );

		SaveBool( main, XorStr( "Misc->AutoJump" ), Misc->AutoJump );
		SaveBool( main, XorStr( "Misc->AutoPistol" ), Misc->AutoPistol );
		SaveInt( main, XorStr( "Misc->AutoStrafe" ), Misc->AutoStrafe );
		SaveBool( main, XorStr( "Misc->CircleStrafe" ), Misc->CircleStrafe );
		SaveInt( main, XorStr( "Misc->CircleStrafe->CircleStrafeKey" ), Misc->CircleStrafeKey );
		SaveBool( main, XorStr( "Misc->NoSpread" ), Removals->NoSpread );
		SaveBool( main, XorStr( "Misc->NoRecoil" ), Removals->NoRecoil );
		SaveBool( main, XorStr( "Misc->NoVisualRecoil" ), Removals->NoVisualRecoil );
		SaveBool( main, XorStr( "Misc->ThirdPerson" ), Misc->ThirdPerson );
		SaveInt( main, XorStr( "Misc->Resolver" ), Misc->Resolver );
		SaveBool( main, XorStr( "Misc->Resolver->AutoFixPitch" ), Misc->AutoFixPitch );
		SaveInt( main, XorStr( "Misc->Resolver->ShotsFired" ), Misc->ShotsFired );
		SaveFloat( main, XorStr( "Misc->Resolver->CorrectioSpeedPower" ), Misc->CorrectioSpeedPower );
		SaveInt( main, XorStr( "Misc->Spam->SpamInChatType" ), Misc->SpamInChat );
		SaveString( main, XorStr( "Misc->Spam->RegularMessage" ), Misc->RegularMessage );
		SaveInt( main, XorStr( "Misc->Spam->SpamTime" ), Misc->SpamTime );
		SaveBool( main, XorStr( "Misc->RoundSay" ), Misc->RoundSay );
		SaveString( main, XorStr( "Misc->RoundSay->RoundStartSay" ), Misc->RoundStartSay );
		SaveString( main, XorStr( "Misc->RoundSay->RoundEndSay" ), Misc->RoundEndSay );
		SaveBool( main, XorStr( "Misc->HitSound" ), Misc->HitSound );
		SaveInt( main, XorStr( "Misc->Menu->BackGroundDarkness" ), Misc->BackGroundDarkness );
	}

	void Delete( const std::string& name )
	{
		auto current = name;

		if( current.find( XorStr( ".ini" ) ) == std::string::npos )
			current.append( XorStr( ".ini" ) );

		m_current = m_config + current;

		DeleteFileA( m_current.c_str() );
	}

	void OnCreateMove()
	{
		auto player = C_CSPlayer::GetLocalPlayer();

		if( !player )
			return;

		auto weapon = player->GetActiveWeapon();

		if( !weapon )
			return;

		if( !Main || !Main->Aimbot || !Main->Triggerbot )
			return;

		auto i = weapon->GetWeaponID();

		if( Main->AimbotWeaponConfig )
			memcpy( Current->Aimbot, Weapon[ i ]->Aimbot, sizeof( AimbotList ) );
		else
			memcpy( Current->Aimbot, Main->Aimbot, sizeof( AimbotList ) );
		
		if( Main->TriggerbotWeaponConfig )
			memcpy( Current->Triggerbot, Weapon[ i ]->Triggerbot, sizeof( TriggerbotList ) );
		else
			memcpy( Current->Triggerbot, Main->Triggerbot, sizeof( TriggerbotList ) );
	}

	auto GetPath() -> std::string
	{
		return m_config;
	}

	const char* WeaponList[ 24 ]
	{
		"P228",
		"Glock-18",
		"Scout",
		"XM1014",
		"MAC-10",
		"AUG",
		"Dual Berettas",
		"Five-SeveN",
		"UMP-45",
		"SG 550",
		"Galil",
		"Famas",
		"USP",
		"AWP",
		"MP5",
		"M249",
		"M3",
		"M4A1",
		"TMP",
		"G3SG1",
		"Desert Eagle",
		"SG 552",
		"AK-47",
		"P90"
	};

	CSWeaponID GetWeaponID( const char* name )
	{
		if( !std::strcmp( name, XorStr( "P228" ) ) )
			return WEAPON_P228;
		else if( !std::strcmp( name, XorStr( "Glock-18" ) ) )
			return WEAPON_GLOCK;
		else if( !std::strcmp( name, XorStr( "Scout" ) ) )
			return WEAPON_SCOUT;
		else if( !std::strcmp( name, XorStr( "XM1014" ) ) )
			return WEAPON_XM1014;
		else if( !std::strcmp( name, XorStr( "MAC-10" ) ) )
			return WEAPON_MAC10;
		else if( !std::strcmp( name, XorStr( "AUG" ) ) )
			return WEAPON_AUG;
		else if( !std::strcmp( name, XorStr( "Dual Berettas" ) ) )
			return WEAPON_ELITE;
		else if( !std::strcmp( name, XorStr( "Five-SeveN" ) ) )
			return WEAPON_FIVESEVEN;
		else if( !std::strcmp( name, XorStr( "UMP-45" ) ) )
			return WEAPON_UMP45;
		else if( !std::strcmp( name, XorStr( "SG 550" ) ) )
			return WEAPON_SG550;
		else if( !std::strcmp( name, XorStr( "Galil" ) ) )
			return WEAPON_GALIL;
		else if( !std::strcmp( name, XorStr( "Famas" ) ) )
			return WEAPON_FAMAS;
		else if( !std::strcmp( name, XorStr( "USP" ) ) )
			return WEAPON_USP;
		else if( !std::strcmp( name, XorStr( "AWP" ) ) )
			return WEAPON_AWP;
		else if( !std::strcmp( name, XorStr( "MP5" ) ) )
			return WEAPON_MP5NAVY;
		else if( !std::strcmp( name, XorStr( "M249" ) ) )
			return WEAPON_M249;
		else if( !std::strcmp( name, XorStr( "M3" ) ) )
			return WEAPON_M3;
		else if( !std::strcmp( name, XorStr( "M4A1" ) ) )
			return WEAPON_M4A1;
		else if( !std::strcmp( name, XorStr( "TMP" ) ) )
			return WEAPON_TMP;
		else if( !std::strcmp( name, XorStr( "G3SG1" ) ) )
			return WEAPON_G3SG1;
		else if( !std::strcmp( name, XorStr( "Desert Eagle" ) ) )
			return WEAPON_DEAGLE;
		else if( !std::strcmp( name, XorStr( "SG 552" ) ) )
			return WEAPON_SG552;
		else if( !std::strcmp( name, XorStr( "AK-47" ) ) )
			return WEAPON_AK47;
		else if( !std::strcmp( name, XorStr( "P90" ) ) )
			return WEAPON_P90;
		else
			return WEAPON_NONE;
	}
}