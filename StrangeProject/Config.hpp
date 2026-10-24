#pragma once

#include "Valve.hpp"
#include <string.h>

#define LimitValue( val, min, max )	if( val < min ) val = min; if( val > max ) val = max;

namespace Config
{
	using Direct3D9::Color;

	struct AimbotList;
	struct TriggerbotList;

	struct MainList
	{
		bool	AimbotWeaponConfig		= false;
		bool	TriggerbotWeaponConfig	= false;

		AimbotList*		Aimbot		= nullptr;
		TriggerbotList*	Triggerbot	= nullptr;
	};

	struct CurrentList
	{
		AimbotList*		Aimbot		= nullptr;
		TriggerbotList*	Triggerbot	= nullptr;
	};

	struct AimbotList
	{
		bool	Active = false;
		int		Key = 0;				// 
		bool	AutoFire = false;		// 
		bool	AutoStop = false;		// 
		bool	AutoCrouch = false;		// 
		bool	AutoReload = false;		// 
		bool	NoSwitch = false;		// 
		int		Spot = 0;				// 
		bool	SpotRandomize = false;	// 
		int		TargetSelection = 0;	// 
		float	FieldOfView = 0.0f;		// 
		int		Smooth = 0;				// 0 - Off | 1 - Step | 2 - Linear
		float	StepX = 0.0f;			// 0 - 100
		float	StepY = 0.0f;			// 0 - 100
		float	SmoothX = 0.0f;			// 0 - 100
		float	SmoothY = 0.0f;			// 0 - 100
		int		Duration = 0;			// ms ( 0 - 5000 )
		int		Delay = 0;				// ms ( 0 - 5000 )
		int		SwitchDelay = 0;		// ms ( 0 - 5000 ) Delay Before Aiming At Next Target
		bool	RCS = false;			// Recoil Control System
		int		RCSDelay = 0;			// bullets ( 0 - 10 )
		int		RCSAmountX = 0;			// percents ( 0 - 100 )
		int		RCSAmountY = 0;			// percents ( 0 - 100 )
		bool	AutoWall = false;		// Aim Through Penetrable Walls
		int		MinDamage = 0;			// Minimum Damage To Deal Through Wall
		int		HitScan = 0;			// 0 - Off | 1 - Normal | 2 - Corners
		float	HitScanScale = 1.0f;	// Corners Scale
		bool	AimAtTeam = false;
		int		Silent = 0;				// 0 - Off | 1 - Normal | 2 - Perfect

		void Clamp()
		{
			LimitValue( Key, 0, 128 );
			LimitValue( FieldOfView, 0.0f, 180.0f );
			LimitValue( StepX, 0.0f, 100.0f );
			LimitValue( StepY, 0.0f, 100.0f );
			LimitValue( SmoothX, 0.0f, 100.0f );
			LimitValue( SmoothY, 0.0f, 100.0f );
			LimitValue( Duration, 0, 5000 );
			LimitValue( Delay, 0, 5000 );
			LimitValue( SwitchDelay, 0, 5000 );
			LimitValue( RCSDelay, 0, 10 );
			LimitValue( RCSAmountX, 0, 100 );
			LimitValue( RCSAmountY, 0, 100 );
			LimitValue( MinDamage, 0, 100 );
			LimitValue( HitScanScale, 0.0f, 1.0f );
		}
	};

	struct TriggerbotList
	{
		int		Mode = 0;				// 0 - Off | 1 - Auto | 2 - On Press
		int		Key = 0;				// 
		int		Accuracy = 0;			// 0 - Normal | 1 - Perfect | 2 - Seed
		int		Delay = 0;				// 0 - 5000 ( ms )
		int		Burst = 0;				// 0 - 10 ( bullets )
		bool	Head = false;			// 
		bool	Chest = false;			// 
		bool	Stomach = false;		// 
		bool	Arms = false;			// 
		bool	Legs = false;			// 
		bool	AutoWall = false;		// 
		int		MinDamage = 0;			// 0 - 100
		int		Target = 0;				// 0 - Everyone | 1 - Enemy | 2 - Friendly

		void Clamp()
		{
			LimitValue( Key, 0, 128 );
			LimitValue( Delay, 0, 5000 );
			LimitValue( Burst, 0, 10 );
			LimitValue( MinDamage, 0, 100 );
		}
	};

	struct ESPList
	{
		bool	Box = false;			// 0 - Off | 1 - Normal | 2 - Corners
		bool	Outlined = false;	// Draw Black Outline
		
		bool	Name = false;		// Draw Player Name
		bool	Weapon = false;		// Draw Player Weapon
		bool	DroppedWeapons = false;
		bool	DroppedC4 = false;
		bool	DroppedGrenades = false;
		bool	AimSpot = false;	// Draw Aim Spot

		bool	Health = false;			// 0 - Off | 1 - Text | 2 - Bar
		bool	Armor = false;			// 0 - Off | 1 - Text | 2 - Bar

		bool	Skeleton = false;	// Draw Player Skeleton
		bool	Colored = false;	// 
		bool	Defusing = true;	// Is Defusing Bomb
		bool	bFixPitchMarker = false;   //	Test
		bool	Bomb = false;		// Show Bomb
		bool	ShowBots = false;		// Show Bomb

		int		Target = 0;			// 0 - Everyone | 1 - Enemy | 2 - Friendly
		float	ViewModelFov = 90;
		float	OverrideFOV = 90;
		void Clamp()
		{
			LimitValue( ViewModelFov, 0.0f, 120.0f);
			LimitValue( OverrideFOV, 0.0f, 140.0f );
		}

	};

	struct RenderList
	{
		bool	ChamsActive = false;
		bool	Wireframe = false;
		bool	HandChams = false;
		bool	NightMode = false;
		int		NightModePower = 100;		
		bool	AsusWalls = false;
		int		AsusWallsPower = 100;
		bool	NoSmoke = false;
		bool	WireframeSmoke = false;
		bool	NightModeTest = false;
		bool	NoFlash = false;
		bool	ChamsColored = false;	// 
		bool	OutOfFovArrovs = false;	// 
		int		ChamsTarget = 0;		// 0 - Everyone | 1 - Enemy | 2 - Friendly
		bool    out = false;
		void Clamp()
		{
			LimitValue( NightModePower, 0, 100 );
			LimitValue( AsusWallsPower, 0, 100 );
		}
	};

	struct AntiAimList
	{
		bool	AtTarget = false;		// 
		float	AtTargetDistance = 8192.0;
		bool	KnifeAntiAim = false;
		bool	EnemyCheck = false;
		bool	EdgeDetection = false;
		bool	bFakeDuck = false;
		bool	FastPlant = false;
		int		SequenceNumber = 0;
		bool	AADisableWithGrenage = true;
		int		LagFixWithAim = 0;
		bool	ClockLagFix = false;
		bool	TimeDemoLagFix = false;

		int		Pitch = 0;				
		int		Yaw = 0;

		int	FakeLag = 0;
		int		ChokedPackets = 1;
		float	PitchCustomAngle = 89.0f;
		int		Factor = 1;
		bool	DisableInAir = false;
		bool	DisableWhileShooting = false;
		bool	DisableWhenFake = false;

		bool    FakeWalk = false;
		int      WalkKey = 0;
		void Clamp( )
		{
			LimitValue( ChokedPackets, 1, 14 );
			LimitValue( AtTargetDistance, 0.0f, 8192.0f );

		}
	};

	struct RemovalsList
	{
		bool	NoSpread = false;			// 0 - Off | 1 - Normal | 2 - Advanced
		bool	SeedHelp = false;		// 
		bool	NoRecoil = false;		// 
		bool	NoVisualRecoil = false;	// 
		
		bool	NoSmoke = false;		// 
		int		FlashAmount = 100;		// 0 - 100 ( percents )

		void Clamp()
		{
			LimitValue( FlashAmount, 0, 100 );
		}
	};

	struct MiscList
	{
		bool	AutoJump = false;		// 
		bool	AutoPistol = false;		// 
		int		AutoStrafe = 0;			// 0 - Off | 1 - Normal | 2 - Boost
		bool	CircleStrafe = false;
		int		CircleStrafeKey = 0;
		int		ThirdPersonKey = 0;
		bool	BombWarning = false;
		bool	ThirdPerson = false;
		bool	AutoBuy = false;
		bool    AutoFixPitch = false;
		bool    HitSound = false;
		bool	RoundSay = false;
		int		SpamInChat = 0;
		char	RegularMessage[256];
		char	RoundStartSay[256];
		char	RoundEndSay[256];
		int		SpamTime = 15;
		int		Resolver = 0;
		int		ShotsFired = 3;
		float	CorrectioSpeedPower = 35.0f;
		int		BackGroundDarkness = 90;
		int		Crosshair = 0;			// 0 - Off | 1 - Dot | 2 - Cross | 3 - Swastika
		bool	Outlined = false;		// 
		bool	ShowRecoil = false;		//
		bool	HitMarkerCalled = false;

		bool	AirStuck = false;		// 
		int		StuckKey = 0;			//

		bool	Speed = false;			// 
		int		SpeedKey = 0;			// 
		int		SpeedFactor = 1;		// 1 - 20 

		int		Restriction = 0;		// 0 - Off | 1 - SMAC | 2 - Ultr@
		int     PointX = 0;
		int     PointY = 0;

		float   viewmodel = 60.0f;
		bool	gamenewmap = false;

		void Clamp()
		{
			LimitValue( SpamTime, 5, 40 );
			LimitValue( ShotsFired, 1, 10 );
			LimitValue( CorrectioSpeedPower, 20, 200 );
			LimitValue( BackGroundDarkness, 0, 100 );
		}
	};

	struct ColorsList
	{
		Color T_ESP_Normal = Color( 255, 0, 0, 255 );		// 
		Color T_ESP_Colored = Color( 255, 255, 0, 255 );	// 

		Color T_Chams_Normal = Color( 255, 0, 0, 255 );		// 
		Color T_Chams_Colored = Color( 255, 0, 0, 255 );	// 
		
		Color CT_ESP_Normal = Color( 0, 128, 255, 255 );	// 
		Color CT_ESP_Colored = Color( 0, 255, 0, 255 );		// 

		Color CT_Chams_Normal = Color( 0, 255, 0, 255 );	// 
		Color CT_Chams_Colored = Color( 0, 255, 0, 255 );	// 

		Color Crosshair = Color( 255, 128, 0, 255 );		// crosshair color
		Color Main = Color( 255, 128, 0, 255 );				// main menu color
	};

	struct BindsList
	{
		int	Menu		= 45;		// VK_INSERT
		int Eject		= 123;		// VK_F11
		int Panic		= 123;		// VK_F12
	};

	extern MainList*		Main;
	extern CurrentList*		Current;
	extern CurrentList*		Weapon[ WEAPON_MAX ];

	extern ESPList*			ESP;
	extern RenderList*		Render;
	extern AntiAimList*		AntiAim;
	extern RemovalsList*	Removals;
	extern MiscList*		Misc;
	extern ColorsList*		Colors;
	extern BindsList*		Binds;

	extern void				Startup( HMODULE hMod );
	extern void				Release();

	extern void				Load( const std::string& name );
	extern void				Save( const std::string& name );
	extern void				Delete( const std::string& name );

	extern void				OnCreateMove();

	extern auto				GetPath() -> std::string;

	extern const char*		WeaponList[ 24 ];
	extern CSWeaponID		GetWeaponID( const char* name );
}