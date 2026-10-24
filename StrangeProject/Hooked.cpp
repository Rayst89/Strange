#include "Hooked.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"
#include "Valve.hpp"
#include "Color.hpp"
#include "Render.hpp"
#include <intrin.h>
#include "Core.hpp"
#include <playsoundapi.h>

#include "d3d9.h"
#include "ImGui/imgui.h"
#include <fstream>
#include <iostream>
#include <string>

bool bSendPacket = false;
Vector3 SavedAngles;
CUserCmd* cmd;
bool g_FixPitch = false;
bool g_CorrectAngles = false;
bool g_SwithPitch = false;
bool g_ResolverLeft;
bool g_ResolverRight;
bool g_ResolverBackWard;
bool g_ResolverBrute1;
bool g_ResolverBrute2;
bool g_ResolverBrute3;
bool g_ResolverBrute4;
bool g_ResolverBrute5;
bool g_ResolverBrute6;
bool g_ResolverBrute7;
bool breaking_lag_compensation;
static Vector3 last_sent_origin;
IGameEventListener2 *pListener;
#define	CONTENTS_EMPTY			0
#define	CONTENTS_SOLID			0x1
#define	CONTENTS_WINDOW			0x2
#define	CONTENTS_AUX			0x4
#define	CONTENTS_GRATE			0x8
#define	CONTENTS_SLIME			0x10
#define	CONTENTS_WATER			0x20
#define	CONTENTS_BLOCKLOS		0x40
#define CONTENTS_OPAQUE			0x80
#define	LAST_VISIBLE_CONTENTS	0x80
#define ALL_VISIBLE_CONTENTS	(LAST_VISIBLE_CONTENTS | (LAST_VISIBLE_CONTENTS-1))
#define CONTENTS_TESTFOGVOLUME	0x100
#define CONTENTS_UNUSED			0x200
#define CONTENTS_UNUSED6		0x400
#define CONTENTS_TEAM1			0x800
#define CONTENTS_TEAM2			0x1000
#define CONTENTS_IGNORE_NODRAW_OPAQUE	0x2000
#define CONTENTS_MOVEABLE		0x4000
#define	CONTENTS_AREAPORTAL		0x8000
#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_MONSTERCLIP	0x20000
#define	CONTENTS_CURRENT_0		0x40000
#define	CONTENTS_CURRENT_90		0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP		0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000
#define	CONTENTS_ORIGIN			0x1000000
#define	CONTENTS_MONSTER		0x2000000
#define	CONTENTS_DEBRIS			0x4000000
#define	CONTENTS_DETAIL			0x8000000
#define	CONTENTS_TRANSLUCENT	0x10000000
#define	CONTENTS_LADDER			0x20000000
#define CONTENTS_HITBOX			0x40000000

#define	MASK_SOLID					(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE)
void Normalize( Vector3 &vIn, Vector3 &vOut )
{
	float flLen = vIn.Length();

	if ( flLen == 0 )
	{
		vOut.Set( 0, 0, 1 );
		return;
	}

	flLen = 1 / flLen;

	vOut.Set( vIn.x * flLen, vIn.y * flLen, vIn.z * flLen );
}

float GetTraceFractionWorldProps( Vector3 startpos, Vector3 endpos, C_CSPlayer* Local )
{

	Ray_t ray;
	trace_t tr;
	CTraceFilterSimple filter( Local );

	ray.Set( startpos, endpos );

	Source::m_pEngineTrace->TraceRay( ray, MASK_SHOT, &filter, &tr );

	return tr.fraction;
}

void RemoveTexture()
{
	IMaterial* pFlash = Source::m_pMatSystem->FindMaterial( "effects\\flashbang", "ClientEffect textures" );
	IMaterial *pfog = Source::m_pMatSystem->FindMaterial( "particle\\screenspace_fog", NULL );
	IMaterial *pSmokeEffect = Source::m_pMatSystem->FindMaterial( "particle\\particle_smokegrenade1", NULL );
	IMaterial* pFlash2 = Source::m_pMatSystem->FindMaterial( "effects\\flashbang_white", "ClientEffect textures" );
	if ( Config::Render->NoSmoke )
	{
		pSmokeEffect->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( true ) );//nosmoke
		pfog->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( true ) );//nofog
	}
	else
	{
		pSmokeEffect->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( false ) );//nosmoke
		pfog->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( false ) );//nofog
	}

	if ( Config::Render->NoFlash )
	{
		pFlash->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( true ) );//noflash
		pFlash2->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( true ) );//noflash
	}
	else
	{
		pFlash->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( false ) );//noflash
		pFlash2->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( false ) );//noflash
	}

	if ( Config::Render->WireframeSmoke )
	{
		pSmokeEffect->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, ( true ) );//nosmoke
		pfog->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( true ) );//nofog
	}
	else
	{
		pSmokeEffect->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, ( false ) );//nosmoke
		pfog->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW, ( false ) );//nofog
	}
}
//std::vector<IMaterial*> vecMapMaterials;
//void NightMode()
//{
//	auto LocalPlayer = C_CSPlayer::GetLocalPlayer();
//	if ( !Source::m_pEngine->IsConnected() || !Source::m_pEngine->IsInGame() || !LocalPlayer )
//	{
//		vecMapMaterials.clear(); 
//		return;
//	}
//
//	for ( MaterialHandle_t h = 0; h < Source::m_pMatSystem->GetNumMaterials(); h++ )
//	{
//		IMaterial *pFirstMaterial = Source::m_pMatSystem->GetMaterial( h );
//
//		if ( !pFirstMaterial )
//			continue;
//
//		if ( strstr( pFirstMaterial->GetTextureGroupName(), "World" ) )
//			vecMapMaterials.push_back( pFirstMaterial );
//	}
//
//	if ( !vecMapMaterials.empty() )
//	{
//		for ( int i = 0; i < vecMapMaterials.size(); i++ )
//		{
//			if ( !vecMapMaterials[ i ] )
//				continue;
//
//			vecMapMaterials[ i ]->AlphaModulate( 0.7 );
//		}
//	}
//
//	if ( Config::Misc->gamenewmap )
//	{
//		vecMapMaterials.clear();
//	}
//
//	if ( vecMapMaterials.empty() )
//	{
//		printf_s( "vecMapMaterial is EMPTY \n" );
//	}
//
//	else
//	{
//		printf_s( "vecMapMaterial is not EMPTY \n" );
//	}
//}
//
//void AsusWalls()
//{
//	for ( MaterialHandle_t i = Source::m_pMatSystem->FirstMaterial(); i != Source::m_pMatSystem->InvalidMaterial(); i = Source::m_pMatSystem->NextMaterial( i ) )
//	{
//		IMaterial *pMaterial = Source::m_pMatSystem->GetMaterial( i );
//
//		if ( !pMaterial )
//			continue;
//
//		if ( strstr( pMaterial->GetTextureGroupName(), "World" ) )
//		{
//			pMaterial->AlphaModulate( Config::Render->AsusWallsPower * 0.01 );
//		}
//	}
//}


void ApplyStrafe( CUserCmd* cmd, const Vector3& va )
{
	float yaw, speed;

	Vector3& move = *( Vector3* )&cmd->forwardmove;

	speed = move.Length2D();

	yaw = ToDegrees( atan2( move.y, move.x ) );
	yaw = ToRadians( cmd->viewangles.y - va.y + yaw );

	move.x = cos( yaw ) * speed;
}

void NormalizeAngles( Vector3 angles )
{
	while ( angles.x > 180.0 )
		angles.x -= 360.0;

	while ( angles.x < -180.0 )
		angles.x += 360.0;

	while ( angles.y > 180.0 )
		angles.y -= 360.0;

	while ( angles.y < -180.0 )
		angles.y += 360.0;

	while ( angles.z > 180.0 )
		angles.z -= 360.0;

	while ( angles.z < -180.0 )
		angles.z += 360.0;
}

void AutoStrafe( CUserCmd* cmd, C_CSPlayer* player )
{
	if( player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP )
		return;

	if( Source::m_pDataManager->GetFlags() & FL_ONGROUND )
		return;

	auto velocity = player->m_vecVelocity();

	float speed = velocity.Length2D();

	float tickrate = 1.0f;

	if( ( Source::m_pGlobalVars->interval_per_tick * 100 ) > 1.0f )
		tickrate = 1.1f;

	static float yaw;

	yaw = AngleNormalize( cmd->viewangles.y - yaw );

	Vector3 strafe = cmd->viewangles;

	if( Config::Misc->AutoStrafe == 1 ) // Normal
	{
		if( yaw < 0.0f )
			cmd->sidemove = 400.0f;
		else if( yaw > 0.0f )
			cmd->sidemove = -400.0f;
	}
	else if( Config::Misc->AutoStrafe == 2 ) // Boost
	{
		float value = ( 8.15f - tickrate ) - ( speed / 340.0f );

		if( speed > 160.0f && speed < 420.0f )
			value = ( 4.6f - tickrate ) - ( speed / 340.0f );

		if( speed > 420.0f )
			value = ( 3.0f - tickrate ) - ( speed / 1000.0f );

		if( value <= 0.275f )
			value = 0.275f;

		if( abs( yaw ) < value )
		{
			static bool direction = false;

			if( direction )
			{
				strafe.y -= value;
				cmd->sidemove = -400.0f;
			}
			else
			{
				strafe.y += value;
				cmd->sidemove = 400.0f;
			}

			direction = !direction;

			ApplyStrafe( cmd, strafe );
		}
		else
		{
			if( yaw < 0.0f )
				cmd->sidemove = 400.0f;
			else if( yaw > 0.0f )
				cmd->sidemove = -400.0f;
		}
	}

	yaw = cmd->viewangles.y;
}


#define TIME_TO_TICKS( dt )    ( ( int )( 0.5f + ( float )( dt ) / I::Globals->interval_per_tick ) )
void ChatSpamRegular()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	int spamtime = Config::Misc->SpamTime;
	double timeSoFar = ( double )( clock() - start_t ) / CLOCKS_PER_SEC;
	if ( timeSoFar < spamtime )
		return;

	static bool holzed = true;
	if ( Config::Misc->RegularMessage[ 0 ] != 0 )
	{
		Source::SayInChat( Config::Misc->RegularMessage );
	}
	

	start_t = clock();
}

void EdgeDetect( CUserCmd* cmd, C_CSPlayer* pLocal, C_WeaponCSBaseGun* weapon )
{
	if ( !pLocal )
		return;

	if ( ( cmd->buttons & IN_ATTACK ) && weapon->IsFireTime() )
		return;

	Vector3 localPosition = pLocal->EyePosition();

	for ( int y = 0; y < 360; y++ )
	{
		Vector3 tmp( 10.0f, cmd->viewangles.y, 0.0f );
		tmp.y += y;
		NormalizeAngles( tmp );

		Vector3 forward;
		AngleVectors( tmp, &forward );

		float length = ( ( 16.0f + 3.0f ) + ( ( 16.0f + 3.0f ) * sin( ToRadians( 10.0f ) ) ) ) + 7.0f;
		forward *= length;

		Ray_t ray;
		trace_t traceData;
		CTraceFilterSimple trace( pLocal );

		ray.Set( localPosition, ( localPosition + forward ) );
		Source::m_pEngineTrace->TraceRay( ray, 0x200400B, &trace, &traceData );

		if ( traceData.fraction != 1.0f )
		{
			Vector3 angles;
			Vector3 Negate = traceData.plane.normal;

			Negate *= -1;
			VectorAngles( Negate, angles );

			tmp.y = angles.y;
			NormalizeAngles( tmp );
			trace_t leftTrace, rightTrace;

			Vector3 left, right;
			AngleVectors( tmp + Vector3( 0.0f, 45.0f, 0.0f ), &left );
			AngleVectors( tmp - Vector3( 0.0f, 45.0f, 0.0f ), &right );

			left *= ( length + ( length * sin( ToRadians( 30.0f ) ) ) );
			right *= ( length + ( length * sin( ToRadians( 30.0f ) ) ) );

			ray.Set( localPosition, ( localPosition + left ) );
			Source::m_pEngineTrace->TraceRay( ray, 0x200400B, 0, &leftTrace );

			ray.Set( localPosition, ( localPosition + right ) );
			Source::m_pEngineTrace->TraceRay( ray, 0x200400B, 0, &rightTrace );

			if ( ( leftTrace.fraction == 1.0f )
				&& ( rightTrace.fraction != 1.0f ) )
			{
				tmp.y -= 45.0f;	
				cmd->viewangles.x = 145.0f;
				// LEFT
			}
			else if ( ( leftTrace.fraction != 1.0f )
				&& ( rightTrace.fraction == 1.0f ) )
			{
				tmp.y += 45.0f;
				cmd->viewangles.x = 145.0f;
				// RIGHT
			}

			cmd->viewangles.y = tmp.y;
		}
	}
}

void AutoJump( CUserCmd* cmd, C_CSPlayer* player )
{
	if( player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP )
		return;

	static bool bFirstJump = false;
	static bool bFakeJump = false;

	if( cmd->buttons & IN_JUMP )
	{
		if( !bFirstJump )
		{
			bFirstJump = bFakeJump = true;
		}
		else if( !( Source::m_pDataManager->GetFlags() & FL_ONGROUND ) )
		{
			if( bFakeJump && player->m_vecVelocity().z < 0.0f )
				bFakeJump = false;
			else
				cmd->buttons &= ~IN_JUMP;
		}
		else
		{
			bFakeJump = true;
		}
	}
	else
	{
		bFirstJump = false;
	}
}

void AutoPistol( CUserCmd* cmd, C_WeaponCSBaseGun* weapon )
{
	if( weapon->GetWeaponType() != WEAPONTYPE_PISTOL &&
		weapon->GetWeaponType() != WEAPONTYPE_SHOTGUN &&
		weapon->GetWeaponType() != WEAPONTYPE_SNIPER_RIFLE )
		return;

	if( !weapon->IsFireTime() )
		cmd->buttons &= ~IN_ATTACK;
}

void AntiSMAC( CUserCmd* cmd )
{
	static Vector3 backup = cmd->viewangles;

	Vector3 angle = cmd->viewangles - backup;
	AnglesNormalize( angle );

	float change = angle.Length();

	if( change > 40.0f )
	{
		cmd->viewangles = backup + angle * 40.0f;
		AnglesNormalize( cmd->viewangles );

		cmd->buttons &= ~IN_ATTACK;
	}

	backup = cmd->viewangles;
}

void AdaptiveFakeLag( CUserCmd* cmd )
{
	bool infakelag = false;
	int flTimer = -1;
	auto Velocity = C_CSPlayer::GetLocalPlayer()->m_vecVelocity().Length2D() * Source::m_pGlobalVars->interval_per_tick;
	int factor = Config::AntiAim->ChokedPackets / 13;
	auto tmpLagTicks = Config::AntiAim->ChokedPackets / 13;
	bool lastfltick = -1;
	do
	{
		factor = tmpLagTicks;
		if ( ( tmpLagTicks * Velocity ) > 68.f )
			break;
		tmpLagTicks += 1;
		if ( ( ( tmpLagTicks + 1 ) * Velocity ) > 68.f )
			break;
		tmpLagTicks += 2;
	}
	while ( tmpLagTicks <= 12 );
	if ( flTimer > factor )
	{
		bSendPacket = true;
		infakelag = false;
		flTimer = -1;
	}
	else
	{
		if ( flTimer == factor )
			lastfltick = true;
		bSendPacket = false;
		infakelag = true;
	}
	flTimer++;
}

void FakeLagWithLC( CUserCmd* cmd )
{
	if ( Config::AntiAim->DisableWhileShooting && C_CSPlayer::GetLocalPlayer()->m_iShotsFired() >= 1 )
		return;

	static auto packets_choked = 0;
	{
		bSendPacket = packets_choked >= Config::AntiAim->ChokedPackets || breaking_lag_compensation;
		!bSendPacket ? ++packets_choked : packets_choked = 0;
	}

	if ( bSendPacket )
	{
		SavedAngles = cmd->viewangles;
		last_sent_origin = C_CSPlayer::GetLocalPlayer()->m_vecOrigin();
	}
}

void FakeLag( CUserCmd* cmd, C_WeaponCSBaseGun* weapon )
{
	if ( Config::AntiAim->DisableWhileShooting && C_CSPlayer::GetLocalPlayer()->m_iShotsFired() >= 1 )
		return;

	static int fake_tick_count = 0;

	if( fake_tick_count == 0 )
	{
		fake_tick_count = Config::AntiAim->ChokedPackets;

		bSendPacket = true;
	}
	else
	{
		fake_tick_count--;

		bSendPacket = false;
	}
}


bool EnemyCheck( C_CSPlayer* LocalPlayer )
{
	auto best = -1;
	auto distance = Config::AntiAim->AtTargetDistance;

	Vector3 end, aim;

	for ( int i = 1; i <= Source::m_pEngine->GetMaxClients(); i++ )
	{
		auto enemy = ToCSPlayer( Source::m_pEntList->GetBaseEntity( i ) );

		if ( !enemy )
			continue;

		if ( enemy == LocalPlayer )
			continue;

		if ( enemy->m_lifeState() != LIFE_ALIVE )
			continue;

		if ( enemy->IsDormant() )
			continue;

		if ( LocalPlayer->m_iTeamNum() == enemy->m_iTeamNum() )
			continue;

		if ( !enemy->GetHitboxVector( 12, end ) )
			continue;

		auto current = end.DistTo( LocalPlayer->EyePosition() );

		if ( current < distance )
		{
			distance = current;
			best = i;
		}
	}

	if ( best != -1 )
	{
		return true;
	}
	else
	{
		return false;
	}
}


void AtTarget( CUserCmd* cmd, C_CSPlayer* player )
{
	auto best = -1;
	auto distance = Config::AntiAim->AtTargetDistance;

	Vector3 end, aim;

	for( int i = 1; i <= Source::m_pEngine->GetMaxClients(); i++ )
	{
		auto enemy = ToCSPlayer( Source::m_pEntList->GetBaseEntity( i ) );

		if( !enemy )
			continue;

		if( enemy == player )
			continue;

		if( enemy->m_lifeState() != LIFE_ALIVE )
			continue;

		if( enemy->IsDormant() )
			continue;

		if ( player->m_iTeamNum() == enemy->m_iTeamNum() )
			continue;

		if( !enemy->GetHitboxVector( 12, end ) )
			continue;

		auto current = end.DistTo( player->EyePosition() );

		if( current < distance )
		{
			distance = current;
			best = i;
		}
	}

	if( best != -1 )
	{
		auto direction = end - player->EyePosition();

		VectorNormalize( direction );
		VectorAngles( direction, aim );

		cmd->viewangles.y = aim.y;

	}
}
void AntiAims( CUserCmd* cmd, C_CSPlayer* pLocal )
{
	static bool switch2 = false;
	static bool flip;
	static bool flip2;
	flip = !flip;
	flip2 = !flip2;

	if ( Config::AntiAim->Pitch == 1 ) // Safe Up
	{
		cmd->viewangles.x = -89.0f;
	}
	else if ( Config::AntiAim->Pitch == 2 ) // Safe Down
	{
		cmd->viewangles.x = 89.0f;
	}
	else if ( Config::AntiAim->Pitch == 3 ) // Fake Down
	{
		cmd->viewangles.x = 180.0f;
	}
	else if ( Config::AntiAim->Pitch == 4 ) // Fake Pitch
	{
		cmd->viewangles.x = -89.f;
		if ( bSendPacket )
			cmd->viewangles.x = 50.f;
	}

	else if ( Config::AntiAim->Pitch == 5 ) // Adaptive Pitch
	{
		cmd->viewangles.x = -189.0f;
		if ( bSendPacket )
			cmd->viewangles.x = 145.0f;
	}

	else if ( Config::AntiAim->Pitch == 6 )
	{
		float moving = abs( pLocal->m_vecVelocity().Length() >= 2 );
		if ( bSendPacket )
		{

			if ( !moving )
			{
				cmd->viewangles.x = 160.f;
			}
			else
			{
				cmd->viewangles.x = 179.f;
			}
		}
		else
		{
			if ( abs( pLocal->m_vecVelocity().Length() < 2 ) )
			{
				cmd->viewangles.x = 99.0f;
			}
			else if ( !moving )
			{
				cmd->viewangles.x = 163.f;
			}
			else	cmd->viewangles.x = 120;


		}
	}

	if ( Config::AntiAim->Yaw == 1 ) // Fake BackWard
	{
		srand( time( NULL ) );
		if ( bSendPacket )
			cmd->viewangles.y += 180.0f;
		else
			cmd->viewangles.y -= 90 + rand() % 180;
	}
	else if ( Config::AntiAim->Yaw == 2 ) // Fake Sideways
	{
		static int choked_tick_count = 0;

		if ( choked_tick_count == 0 )
		{
			choked_tick_count = 5;

			bSendPacket = true;

			cmd->viewangles.y -= 90.0f;
		}
		else
		{
			choked_tick_count--;

			bSendPacket = false;

			cmd->viewangles.y += 90.0f;
		}
	}
	else if ( Config::AntiAim->Yaw == 3 )  // Fake1
	{
		if ( bSendPacket )
		{
			cmd->viewangles.y += 180;
		}
		else
		{
			cmd->viewangles.y += 90;
		}
	}
	else if ( Config::AntiAim->Yaw == 4 )  // Jitter
	{
		int type = cmd->command_number % 3;

		if ( type == 1 )
		{
			cmd->viewangles.y -= 160.0f;
			bSendPacket = true;
		}
		else if ( type == 2 )
		{
			cmd->viewangles.y -= 200.0f;
			bSendPacket = false;
		}
	}
	else if ( Config::AntiAim->Yaw == 5 )	// Dunamic Fake
	{
		if ( Config::AntiAim->FakeLag && ( Config::AntiAim->ChokedPackets != 0 ) )
		{
			if ( flip )
			{
				if ( flip2 )
					cmd->viewangles.y += 90.0f;
				else
					cmd->viewangles.y -= 90.0f;
			}
			else
			{
				cmd->viewangles.y += 180.0f;
			}
		}
		else
		{
			if ( flip )
			{
				bSendPacket = false;
				if ( flip2 )
					cmd->viewangles.y += 90.0f;
				else
					cmd->viewangles.y -= 90.0f;
			}
			else
			{
				bSendPacket = true;
				cmd->viewangles.y += 180.0f;
			}
		}
	}
	else if ( Config::AntiAim->Yaw == 6 )
	{
		if ( switch2 )
		{
			cmd->viewangles.y += -150;
		}
		else
		{
			cmd->viewangles.y += 150;
		}
		switch2 = !switch2;
	}
}

void AntiAim( CUserCmd* cmd, C_CSPlayer* player, C_WeaponCSBaseGun* weapon )
{
	auto WeaponID = weapon->GetWeaponID();
	if ( player->m_MoveType() == MOVETYPE_LADDER || player->m_MoveType() == MOVETYPE_NOCLIP )
		return;

	if ( ( cmd->buttons & IN_ATTACK ) && weapon->IsFireTime() )
		return;

	if ( !Config::AntiAim->KnifeAntiAim && WeaponID == WEAPON_KNIFE )
		return;

	if ( WeaponID == WEAPON_HEGRENADE || WeaponID == WEAPON_FLASHBANG || WeaponID == WEAPON_SMOKEGRENADE )
		return;

	if ( EnemyCheck( player ) == false && Config::AntiAim->EnemyCheck == true )
		return;


	if( Config::AntiAim->AtTarget )
		AtTarget( cmd, player );

	
	AntiAims( cmd, player );
}

void PerfectSilent( CUserCmd* cmd, C_WeaponCSBaseGun* weapon )
{
	if( cmd->buttons & IN_ATTACK && weapon->IsFireTime() )
		bSendPacket = false;
}

void AirStuck( CUserCmd* cmd, C_WeaponCSBaseGun* weapon )
{
	if( !Shared::m_bStuck )
		return;

	if( cmd->buttons & IN_ATTACK && weapon->IsFireTime() )
		return;

	cmd->tick_count = 0xFFFFF;
}
void FakeWalk(CUserCmd* cmd, C_CSPlayer* player)
{
		if (GetAsyncKeyState(Config::AntiAim->WalkKey))
		{

			static int iChoked = -1;
			iChoked++;

			if (iChoked < 3)
			{
				bSendPacket = false;
				cmd->tick_count += 10;
				cmd += 7 + cmd->tick_count % 2 ? 0 : 1;
				cmd->buttons |= player->m_MoveType() == IN_BACK;
				cmd->forwardmove = cmd->sidemove = 0.f;
			}
			else
			{
				bSendPacket = true;
				iChoked = -1;
				Source::m_pGlobalVars->frametime *= (player->m_vecVelocity().Length2D()) / 1.f;
				cmd->buttons |= player->m_MoveType() == IN_FORWARD;
			}

		}
}
void RemoveInterpolation()
{
	// 0x390028 client.dll
	static std::uintptr_t interpolation_list = ( std::uintptr_t )GetImageBase( XorStr( "client.dll" ) ) + 0x390028;

	*( std::uint16_t* )( interpolation_list + 12 ) = 0xFFFF;
	*( std::uint16_t* )( interpolation_list + 18 ) = 0;
}

void __fastcall CreateMove( void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active )
{
	auto player = C_CSPlayer::GetLocalPlayer();
	auto netchannel = Source::m_pEngine->GetNetChannelInfo();
	static auto module_engine = ( DWORD )GetModuleHandleA( "engine.dll" );
	__try
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >( IBaseClientDLL_CreateMove )( ecx, sequence_number, input_sample_frametime, active );

		bSendPacket = true;

		auto cmd = Source::m_pInput->GetUserCmd( sequence_number );

		

		if( cmd )
		{
			Config::OnCreateMove();


			if( player )
			{
				Source::m_pDataManager->PreCreateMove( cmd, player );

				Source::m_pPlayerList->OnCreateMove();

				if( player->m_lifeState() == LIFE_ALIVE )
				{
					if( Config::Misc->AutoJump )
						AutoJump( cmd, player );

					if( Config::Misc->AutoStrafe )
						AutoStrafe( cmd, player );

					auto weapon = player->GetActiveWeapon();

					if( weapon )
					{
						auto va = cmd->viewangles;
						if ( !weapon->IsMelee() && weapon->m_iClip1() )
						{
							if ( Config::Current->Aimbot->Active )
							{
								Source::m_pAimbot->ExecuteAimbot( cmd );
							}
						}


						if ( Config::AntiAim->FakeLag == 1 )
						{
							FakeLag( cmd, weapon );
						}

						if ( Config::AntiAim->FakeLag == 2 )
						{
							FakeLagWithLC( cmd );
						}

						if ( Config::AntiAim->ClockLagFix )
						{
							Source::m_pCvar->FindVar( "cl_clock_correction_force_server_tick" )->SetValue(7);
							Source::m_pCvar->FindVar( "cl_clock_correction_adjustment_min_offset" )->SetValue(128);
							Source::m_pCvar->FindVar( "cl_clock_correction_adjustment_max_offset" )->SetValue(128);
							Source::m_pCvar->FindVar( "cl_clockdrift_max_ms" )->SetValue(0);
							Source::m_pCvar->FindVar( "cl_smooth" )->SetValue(0);
							Source::m_pCvar->FindVar( "cl_extrapolate_amount" )->SetValue(0);
							Source::m_pCvar->FindVar( "sv_client_interpolate" )->SetValue(0);
						}
						else
						{
							Source::m_pCvar->FindVar( "cl_clock_correction_force_server_tick" )->SetValue( 999 );
							Source::m_pCvar->FindVar( "cl_clock_correction_adjustment_min_offset" )->SetValue( 10 );
							Source::m_pCvar->FindVar( "cl_clock_correction_adjustment_max_offset" )->SetValue( 90 );
							Source::m_pCvar->FindVar( "cl_clockdrift_max_ms" )->SetValue( 150 );
							Source::m_pCvar->FindVar( "cl_smooth" )->SetValue( 1 );
							Source::m_pCvar->FindVar( "cl_extrapolate_amount" )->SetValue( 0.25 );
							Source::m_pCvar->FindVar( "sv_client_interpolate" )->SetValue( -1 );
						}

						if( Config::AntiAim->FakeWalk )
							FakeWalk(cmd, player);

						if ( Config::Removals->NoSpread )
							Source::m_pAccuracy->NoSpread( cmd, weapon );

						if ( Config::Misc->CircleStrafe && GetAsyncKeyState(Config::Misc->CircleStrafeKey))
						{
							if ( !( Source::m_pDataManager->GetFlags() & FL_ONGROUND ) )
							{
								static float StrafeAngle;

								Vector3 Velocity = C_CSPlayer::GetLocalPlayer()->m_vecVelocity();
								Velocity.z = 0;

								float Speed = Velocity.Length();
								if ( Speed < 45 ) Speed = 45;
								if ( Speed > 750 ) Speed = 750;

								float FinalPath = GetTraceFractionWorldProps( C_CSPlayer::GetLocalPlayer()->m_vecOrigin() + Vector3( 0, 0, 10 ), C_CSPlayer::GetLocalPlayer()->m_vecOrigin() + Vector3( 0, 0, 10 ) + Velocity / 2.0f, player );
								float DeltaAngle = max( ( 275.0f / Speed ) * ( 2.0f / FinalPath ) * ( 128.0f / ( 1.7f / Source::m_pGlobalVars->interval_per_tick ) ) * 4.f, 2.0f );
								StrafeAngle += DeltaAngle;

								if ( fabs( StrafeAngle ) >= 360.0f )
								{
									StrafeAngle = 0.0f;
									active = false;
								}
								else
								{
									cmd->forwardmove = cos( ( StrafeAngle + 90 ) * ( M_PI / 180.0f ) ) * 450.f;
									cmd->sidemove = sin( ( StrafeAngle + 90 ) * ( M_PI / 180.0f ) ) * 450.f;
								}
							}
						}

						if ( Config::Removals->NoRecoil )
							Source::m_pAccuracy->NoRecoil( cmd, player, weapon );

						if( Config::Current->Aimbot->Silent == 2 )
							PerfectSilent( cmd, weapon );

						if( cmd->buttons & IN_ATTACK && !weapon->IsFireTime() )
							cmd->viewangles = va;

						Vector3 vVelocity = player->m_vecVelocity();
					
						if( Config::AntiAim->AtTarget || Config::AntiAim->Pitch || Config::AntiAim->Yaw )
							AntiAim( cmd, player, weapon );

						if ( vVelocity.Length() < 300.f )
						{
							if ( Config::AntiAim->EdgeDetection )
							{
								EdgeDetect( cmd, player, weapon );
							}
						}

						if( Config::Misc->AutoPistol )
							AutoPistol( cmd, weapon );

						if( Config::Misc->AirStuck )
							AirStuck( cmd, weapon );

						if( Config::Misc->Restriction == 2 )
							AntiSMAC( cmd );

						if ( Config::Misc->SpamInChat == 1 )
							ChatSpamRegular();

						if (bSendPacket)
							SavedAngles = cmd->viewangles;

						if ( Config::AntiAim->FastPlant)
						{
							bSendPacket = ( sequence_number % 10 ) == 0;
							cmd->command_number += Config::AntiAim->SequenceNumber * MULTIPLAYER_BACKUP;
							*( int* )( ( DWORD )module_engine + 0x39539C ) += Config::AntiAim->SequenceNumber * MULTIPLAYER_BACKUP; // lastoutgoingcommand
							*( int* )( ( DWORD )netchannel + 0x8 ) += Config::AntiAim->SequenceNumber * MULTIPLAYER_BACKUP; // m_nOutSequenceNr
						}

						Source::MovementFix( cmd, va );
					}
				}
				Source::m_pDataManager->PostCreateMove( player );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pClientSwap->VCall< CreateMoveFn >( IBaseClientDLL_CreateMove )( ecx, sequence_number, input_sample_frametime, active );
	}
}

void __declspec( naked ) __fastcall Hooked_CreateMove( void* ecx, void* edx, int sequence_number, float input_sample_frametime, bool active )
{
	__asm
	{
		push    ebp
		mov     ebp, esp

		mov     bSendPacket, bl

		movzx	eax, active
		push	eax
		mov		eax, input_sample_frametime
		push	eax
		mov		eax, sequence_number
		push	eax
		call    CreateMove

		mov     bl, bSendPacket

		mov     esp, ebp
		pop     ebp

		retn    0xC
	}
}

void __fastcall Hooked_FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t curStage )
{
	__try
	{
		Vector3 saved;
		auto player = C_CSPlayer::GetLocalPlayer();

		if( curStage == FRAME_RENDER_START )
		{
			if ( player )
			{
				saved = player->m_vecPunchAngle();
				Vector3 view;
				if ( Config::Removals->NoVisualRecoil )
				{
					player->m_vecPunchAngle().Set();
				}

				if ( Source::m_pInput->CAM_IsThirdPerson() )
				{
					auto dwDeadFlag = Source::m_pNetVarManager->Get( XorStr("DT_CSPlayer"), XorStr("deadflag") );
					*reinterpret_cast< Vector3* >( reinterpret_cast< DWORD >( C_CSPlayer::GetLocalPlayer() ) + dwDeadFlag + 4 ) = SavedAngles;
				}

				if ( Config::Misc->ThirdPerson == true)
				{
					Source::m_pInput->CAM_ToThirdPerson();
					Source::m_pCvar->FindVar( XorStr( "sv_cheats" ) )->SetValue( 1 );
					Source::m_pCvar->FindVar( XorStr( "cam_idealyaw" ) )->SetValue( 0 );
					if ( player->m_lifeState() != LIFE_ALIVE )
					{
						Source::m_pInput->CAM_ToFirstPerson();
					}
				}
				else if ( Config::Misc->ThirdPerson == false )
				{
					Source::m_pInput->CAM_ToFirstPerson();
					Source::m_pCvar->FindVar( XorStr( "sv_cheats" ) )->SetValue( 0 );
				}

				player->m_flFlashMaxAlpha() = ( float )Config::Removals->FlashAmount * 2.55f;
			}
		}

		Source::m_pClientSwap->VCall< FrameStageNotifyFn >( IBaseClientDLL_FrameStageNotify )( ecx, curStage );

		if( curStage == FRAME_RENDER_START )
		{
			if( player )
				player->m_vecPunchAngle() = saved;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pClientSwap->VCall< FrameStageNotifyFn >( IBaseClientDLL_FrameStageNotify )( ecx, curStage );
	}
}

void __fastcall Hooked_RunCommand( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >( IPrediction_RunCommand )( ecx, player, ucmd, moveHelper );

		if( player == C_CSPlayer::GetLocalPlayer() )
			Source::m_pDataManager->OnDataUpdate();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< RunCommandFn >( IPrediction_RunCommand )( ecx, player, ucmd, moveHelper );
	}
}

void __fastcall Hooked_FinishMove( void* ecx, void* edx, C_CSPlayer* player, CUserCmd* ucmd, CMoveData* move )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >( IPrediction_FinishMove )( ecx, player, ucmd, move );

		*( Vector3* )( player + 0xEC ) = player->m_vecVelocity() + player->m_vecBaseVelocity();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< FinishMoveFn >( IPrediction_FinishMove )( ecx, player, ucmd, move );
	}
}

void __fastcall Hooked_Update( void* ecx, void* edx, bool recieved_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command )
{
	__try
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >( IPrediction_Update )( ecx, true, validframe, incoming_acknowledged, outgoing_command );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pPredictionSwap->VCall< UpdateFn >( IPrediction_Update )( ecx, recieved_new_world_update, validframe, incoming_acknowledged, outgoing_command );
	}
}

CUserCmd* __fastcall Hooked_GetUserCmd( void* ecx, void* edx, int sequence_number )
{
	__try
	{
		return Source::m_pInput->GetUserCmd( sequence_number );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return nullptr;
	}
}

void __fastcall Hooked_ResetMouse( void* ecx, void* edx )
{
	__try
	{
		if( !Shared::m_bPanic )
		{
			if( Shared::m_bMenu )
				return;
		}

		Source::m_pInputSwap->VCall< ResetMouseFn >( IInput_ResetMouse )( ecx );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		Source::m_pInputSwap->VCall< ResetMouseFn >( IInput_ResetMouse )( ecx );
	}
}

float __fastcall Hooked_GetViewModelFOV( void* ecx, void* edx )
{
	return Config::ESP->ViewModelFov;
}

void __fastcall Hooked_OverrideView( void* ecx, void* edx, void* pSetup )
{
	if ( Source::m_pEngine->IsInGame() && Source::m_pEngine->IsConnected() )
		*( float* )( ( DWORD )pSetup + 0x28 ) = Config::ESP->OverrideFOV;

	Source::m_pClientModeSwap->VCall< OverrideViewFn >( 16 )(ecx, pSetup ); // 16 index
}

//bool __fastcall Hooked_FireGameEvent( void *ecx, void* edx, IGameEvent* pEvent )
//{
//	const char* szEventName = pEvent->GetName();
//	if (!szEventName )
//		 return Source::m_pEventSwap->VCall< FireEvent >( 8 )( ecx, pEvent);
//
//	 if ( !strcmp( szEventName, "round_start" ) ) //
//	 {
//		 SayInChat("round_start");
//	 }
//
//	 if ( strcmp( szEventName, "player_hurt" ) )
//	 {
//		 SayInChat( "player_hurt" );
//	 }	
//
//	 if ( !strcmp( szEventName, "player_death" ) )
//	 {
//		 SayInChat( "player_death" );
//	 }
//	return Source::m_pEventSwap->VCall< FireEvent >( 8 )( ecx, pEvent );
//}


int __fastcall Hooked_DrawModelEx( void* ecx, void* edx, ModelRenderInfo_t* info )
{
	__try
	{
		if( !Shared::m_bPanic )
		{
			const char* ModelName = Source::m_pModelInfoClient->GetModelName( ( model_t* )info->pModel );
			if ( Config::Render->ChamsActive )
			{
				Source::m_pRender->OnDrawModel( ecx, info );
			}

			int iRet = Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );

			Source::m_pRender->ForceMaterial( Direct3D9::Color::Empty, nullptr, false );

			return iRet;
		}

		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
	}
}

HRESULT D3DAPI Hooked_Reset( IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters )
{
	__try
	{
		Source::m_pRenderer->OnLostDevice();
		Source::m_pMenu->OnLostDevice();

		Source::m_pDeviceSwap->Restore();
		auto hRet = Source::m_pDeviceSwap->VCall< ResetFn >( IDirect3DDevice9_Reset )( pDevice, pPresentationParameters );
		Source::m_pDeviceSwap->Replace();

		Source::m_pRenderer->OnResetDevice();
		Source::m_pMenu->OnResetDevice();
		return hRet;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return Source::m_pDeviceSwap->VCall< ResetFn >( IDirect3DDevice9_Reset )( pDevice, pPresentationParameters );
	}
}

bool __fastcall Hooked_IsPlayingTimeDemo( void* ecx, void* edx )
{
	static auto call = Memory::PatternScan( XorStr( "client.dll" ), XorStr( "84 C0 74 07 C6 05" ) );

	if ( Config::AntiAim->TimeDemoLagFix )
	{
		if ( Source::m_pEngine->IsInGame() || Source::m_pEngine->IsConnected() )
		{
			if ( _ReturnAddress() == ( void* )call )
				return true;
		}
	}

	return Source::m_pEngineSwap->VCall<IsPlayingTimeDemoFn>( 78 )( ecx );
}

bool IsGoodPanel( VPANEL vguiPanel )
{
	static VPANEL MatSystemTopPanel = 0;

	if ( !MatSystemTopPanel )
	{
		const char* pName = Source::m_pPanel->GetName( vguiPanel );

		if ( std::strcmp( pName, "MatSystemTopPanel" ) == 0 )
		{
			MatSystemTopPanel = vguiPanel;
			return true;
		}
	}

	return ( vguiPanel == MatSystemTopPanel );
}

void PaintEntity( C_CSPlayer* player )
{
	using Direct3D9::Color;

	if ( player->IsDormant() )
		return;

	if ( player->m_lifeState() != LIFE_ALIVE )
		return;

	if ( player->m_iTeamNum() == C_CSPlayer::GetLocalPlayer()->m_iTeamNum() )
		return;

	Vector3 head;

	if ( !player->GetHitboxVector( 12, head ) )
		return;

	Color color;

	if ( player->m_iTeamNum() == 2 ) // T
	{
		color = Config::Colors->T_ESP_Normal;

		if ( Config::ESP->Colored && Source::TraceLine( head, player ) )
			color = Config::Colors->T_ESP_Colored;
	}
	else if ( player->m_iTeamNum() == 3 ) // CT
	{
		color = Config::Colors->CT_ESP_Normal;

		if ( Config::ESP->Colored && Source::TraceLine( head, player ) )
			color = Config::Colors->CT_ESP_Colored;
	}

	const auto& transform = player->m_rgflCoordinateFrame();

	Vector3 position( transform[ 0 ][ 3 ], transform[ 1 ][ 3 ], transform[ 2 ][ 3 ] );

	Vector3 mins = player->GetMins();
	Vector3 maxs = player->GetMaxs() + Vector3( 0.0f, 0.0f, 10.0f );

	Vector3 points[] =
	{
		{ mins.x, mins.y, mins.z },
		{ mins.x, maxs.y, mins.z },
		{ maxs.x, maxs.y, mins.z },
		{ maxs.x, mins.y, mins.z },
		{ maxs.x, maxs.y, maxs.z },
		{ mins.x, maxs.y, maxs.z },
		{ mins.x, mins.y, maxs.z },
		{ maxs.x, mins.y, maxs.z },
	};

	Vector3 transformed[ 8 ];

	for ( int i = 0; i < 8; i++ )
		VectorTransform( points[ i ], transform, transformed[ i ] );

	Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

	if ( !Source::WorldToScreen( transformed[ 3 ], flb ) ||
		!Source::WorldToScreen( transformed[ 0 ], blb ) ||
		!Source::WorldToScreen( transformed[ 2 ], frb ) ||
		!Source::WorldToScreen( transformed[ 6 ], blt ) ||
		!Source::WorldToScreen( transformed[ 5 ], brt ) ||
		!Source::WorldToScreen( transformed[ 4 ], frt ) ||
		!Source::WorldToScreen( transformed[ 1 ], brb ) ||
		!Source::WorldToScreen( transformed[ 7 ], flt ) )
		return;

	Vector3 screen[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for ( int i = 0; i < 8; i++ )
	{
		if ( left > screen[ i ].x )
			left = screen[ i ].x;
		if ( top < screen[ i ].y )
			top = screen[ i ].y;
		if ( right < screen[ i ].x )
			right = screen[ i ].x;
		if ( bottom > screen[ i ].y )
			bottom = screen[ i ].y;
	}

	int x = ( int )std::round( left );
	int y = ( int )std::round( bottom );
	int a = ( int )std::round( right );
	int b = ( int )std::round( top );

	int w = ( int )std::round( right - left );
	int h = ( int )std::round( top - bottom );

	if ( Config::ESP->Box )
	{
		Source::m_pRenderer->DrawBorderBoxOut( x, y, w, h, 1, color, Color( 0, 0, 0, color.A ) );
	}

	int pad_h = 0;

	if ( Config::ESP->Name )
	{
		player_info_t data;

		if ( Source::m_pEngine->GetPlayerInfo( player->GetIndex(), &data ) )
			Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color::White, data.name );
	}

	if ( Config::ESP->Weapon )
	{
		auto weapon = player->GetActiveWeapon();

		if ( weapon )
		{
			auto name = weapon->GetName();

			if ( name )
				Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y + h + 2, FONT_ALIGN_CENTER_H, Color::White, "%s", ( name + 7 ) );
		}
	}

	if ( Config::ESP->AimSpot )
	{
		Vector3 spot;

		if ( player->GetHitboxVector( Config::Current->Aimbot->Spot, spot ) )
		{
			Vector3 screen;
			if ( Source::WorldToScreen( spot, screen ) )
				Source::m_pRenderer->DrawRectOut( screen.x - 1, screen.y - 1, 3, 3, Color::White );
		}
	}
	if ( Config::ESP->Health ) // Bar
	{
		int health = player->m_iHealth();

		if ( health > 100 )
			health = 100;

		int size_h = ( int )std::round( ( ( h + 1 ) * health ) / 100 );
		int real_h = ( h + 1 ) - size_h;

		Source::m_pRenderer->DrawRect( x - 6, y - 1, 4, h + 3, Color::Black );
		Source::m_pRenderer->DrawRect( x - 5, y + real_h, 2, size_h, Color( 255 - ( player->m_iHealth() * 2.55f ), player->m_iHealth() * 2.55f, 0 ) );
	}

	if ( Config::ESP->Armor ) // Bar
	{
		int armor = player->m_ArmorValue();

		if ( armor > 100 )
			armor = 100;

		int size_h = ( int )std::round( ( ( h + 1 ) * armor ) / 100 );
		int real_h = ( h + 1 ) - size_h;

		Source::m_pRenderer->DrawRect( x + w + 3, y - 1, 4, h + 3, Color::Black );
		Source::m_pRenderer->DrawRect( x + w + 4, y + real_h, 2, size_h, Color( 180, 180, 180 ) );
	}

	/*if( Config::ESP->Skeleton )
	{
		auto pModel = player->GetModel();

		if( pModel )
		{
			auto pStudioHdr = Source::m_pModelInfoClient->GetStudioModel( pModel );

			if( pStudioHdr )
			{
				for( auto i = 0; i < pStudioHdr->numbones; i++ )
				{
					auto pBone = pStudioHdr->GetBone( i );

					if( !pBone )
						continue;

					if( !( pBone->flags & 0x100 ) )
						continue;

					if( pBone->parent == -1 )
						continue;

					Vector3 vBone, vParent;
					Vector3 vBoneS, vParentS;

					player->GetBoneVector( i, vBone );
					player->GetBoneVector( pBone->parent, vParent );

					if( Source::WorldToScreen( vBone, vBoneS ) && Source::WorldToScreen( vParent, vParentS ) )
						Source::m_pRenderer->DrawLine( vBoneS.x, vBoneS.y, vParentS.x, vParentS.y, Color::White );
				}
			}
		}
	}*/

	if ( Config::ESP->bFixPitchMarker == true )
	{
		player_info_t data;
		if ( Source::m_pEngine->GetPlayerInfo( player->GetIndex(), &data ) )
		{
			if ( data.fakeplayer == true && Config::ESP->bFixPitchMarker == true )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color( 0, 175, 0, 255), "BOT" );
			}
			else if ( g_FixPitch == true )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color( 234, 128, 0, 255), "Pitch Fixed" );
			}
			else if ( g_CorrectAngles == true )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color::Green, "Pitch Correct" );
			}
			else if ( g_SwithPitch == true )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color( 255, 44, 0, 255), "Pitch Switched" );
			}
		}
	}
	else if ( Config::ESP->bFixPitchMarker == true && Config::Misc->AutoFixPitch == false )
	{
		Source::m_pRenderer->DrawText( Source::m_hFont, x + w / 2, y - 16, FONT_ALIGN_CENTER_H, Color( 255, 0, 0, 255 ), "Pitch Fix Off" );
	}
}

float ScaleDamage( float damage, int armor )
{
	float ratio = 0.5f;
	float bonus = 0.5f;

	if( armor > 0 )
	{
		float new_damage = damage * ratio;
		float new_armor = ( damage - new_damage ) * bonus;

		if( new_armor > static_cast< float >( armor ) )
		{
			new_armor = static_cast< float >( armor ) * ( 1.0f / bonus );
			new_damage = damage - new_armor;
		}

		damage = new_damage;
	}

	return damage;
}

void PaintGround( C_BaseEntity* ent )
{
	bool AlwaysActiveTest = true;
	using Direct3D9::Color;

	if( ent->IsDormant() )
		return;

	auto name = ent->GetClientClass()->m_pNetworkName;

	auto world = ent->m_vecOrigin();

	if( world.IsZero() )
		return;

	else if ( Config::ESP->DroppedWeapons )
	{
		Vector3 screen;
		if ( Source::WorldToScreen( world, screen ) )
		{
			auto name = ent->GetClientClass()->m_pNetworkName;
			if ( std::string( name ).find( "CWeaponXM1014" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "XM1014" );
			}

			else if ( std::string( name ).find( "CWeaponUSP" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "USP" );
			} 			
			
			else if ( std::string( name ).find( "CWeaponUMP45" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "UMP45" );
			}			
			
			else if ( std::string( name ).find( "CWeaponTMP" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "TMP" );
			}			
			
			else if ( std::string( name ).find( "CWeaponSG552" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "SG552" );
			}			
			
			else if ( std::string( name ).find( "CWeaponSG550" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "SG550" );
			}			
			
			else if ( std::string( name ).find( "CWeaponScout" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Scout" );
			}			
			
			else if ( std::string( name ).find( "CWeaponP90" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "P90" );
			}			
			
			else if ( std::string( name ).find( "CWeaponP228" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "P228" );
			}			
			
			else if ( std::string( name ).find( "CWeaponMP5Navy" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "MP5Navy" );
			}

			else if ( std::string( name ).find( "CWeaponMAC10" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "MAC10" );
			}

			else if ( std::string( name ).find( "CWeaponM4A1" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "M4A1" );
			}

			else if ( std::string( name ).find( "CWeaponM3" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "M3" );
			}

			else if ( std::string( name ).find( "CWeaponM249" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "M249" );
			}

			else if ( std::string( name ).find( "CWeaponGlock" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Glock" );
			}

			else if ( std::string( name ).find( "CWeaponGalil" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Galil" );
			}

			else if ( std::string( name ).find( "CWeaponG3SG1" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "G3SG1" );
			}

			else if ( std::string( name ).find( "CWeaponFiveSeven" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "FiveSeven" );
			}

			else if ( std::string( name ).find( "CWeaponFamas" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Famas" );
			}

			else if ( std::string( name ).find( "CWeaponElite" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "DualElite" );
			}

			else if ( std::string( name ).find( "CDEagle" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Deagle" );
			}

			else if ( std::string( name ).find( "CWeaponAWP" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "AWP" );
			}

			else if ( std::string( name ).find( "CWeaponAug" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Aug" );
			}

			else if ( std::string( name ).find( "CAK47" ) != std::string::npos )
			{
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_CENTER_H, Color::White, "Ak47" );
			}
		}
	}

	if( std::strcmp( name, XorStr( "CPlantedC4" ) ) == 0 || std::string( name ).find( "CC4" ) != std::string::npos ) //  CPlantedC4
	{
		if( Config::ESP->Bomb )
		{
			Vector3 screen;

			if( Source::WorldToScreen( world, screen ) )
				Source::m_pRenderer->DrawText( Source::m_hFont, screen.x, screen.y, FONT_ALIGN_LEFT, Color::Red, XorStr( "[ C4 ]" ) );
		}

		if( Config::Misc->BombWarning )
		{
			auto player = C_CSPlayer::GetLocalPlayer();

			Vector3 bomb_origin = ent->m_vecOrigin();
			Vector3 spot_top( bomb_origin.x, bomb_origin.y, bomb_origin.z + 8.0f );
			Vector3 spot_bottom( spot_top.x, spot_top.y, spot_top.z - 40.0f );

			CTraceFilterSimple trace( ent );

			Ray_t ray;
			ray.Set( spot_top, spot_bottom );

			trace_t tr;
			Source::m_pEngineTrace->TraceRay( ray, 0x200400B, &trace, &tr );

			float bomb_radius = 500.0f;

			if( tr.fraction != 1.0f )
				bomb_origin = tr.endpos + tr.plane.normal * 0.6f;

			float damage = bomb_radius;
			float radius = bomb_radius * 3.5f;

			float adjusted_damage, falloff, damage_percentage;

			Vector3 src = bomb_origin;
			src.z += 1.0f;

			Vector3 end = player->EyePosition();

			damage_percentage = 1.0f;
			falloff = ( radius != 0.0f ) ? damage / radius : 1.0f;

			Vector3 target = end - src;

			adjusted_damage = ( damage - target.Length() * falloff ) * damage_percentage;
			adjusted_damage = ScaleDamage( adjusted_damage, player->m_ArmorValue() );

			if( adjusted_damage > 0.0f )
			{
				int w, h;
				Source::m_pEngine->GetScreenSize( w, h );

				w /= 2;

				Color color( Color::Red );

				if( adjusted_damage < 80.0f )
					color = Color::Yellow;
				else if( adjusted_damage < 20.0f )
					color = Color::Green;

				Source::m_pRenderer->DrawText( Source::m_hFont, w, 5.0f, FONT_ALIGN_CENTER_H, color, XorStr( "Explosion Warning! [%.2f]" ), adjusted_damage );
			}
		}
	}

}

void ESP()
{
	auto player = C_CSPlayer::GetLocalPlayer();

	if( !player )
		return;

	int size = Source::m_pEntList->GetHighestEntityIndex();

	for( int i = 0; i <= size; i++ )
	{
		auto ent = Source::m_pEntList->GetBaseEntity( i );

		if( !ent )
			continue;

		PaintGround( ent );

		auto enemy = ToCSPlayer( ent );

		if( !enemy )
			continue;

		if( enemy == player )
			continue;

		PaintEntity( enemy );
	}
}

void Crosshair( const Vector3 &vecDelta )
{
	using Direct3D9::Color;
	float flRadius = 360.0f;
	int iScreenWidth, iScreenHeight;
	Source::m_pEngine->GetScreenSize( iScreenWidth, iScreenHeight );

	Vector3 vRealAngles;
	Source::m_pEngine->GetViewAngles( vRealAngles );

	Vector3 vForward, vRight, vUp( 0.0f, 0.0f, 1.0f );

	AngleVectors( vRealAngles, &vForward, NULL, NULL );

	vForward.z = 0.0f;
	NormalizeAngles( vForward );
	CrossProduct( vUp, vForward, vRight );

	float flFront = DotProduct( vecDelta, vForward );
	float flSide = DotProduct( vecDelta, vRight );
	float flXPosition = flRadius * -flSide;
	float flYPosition = flRadius * -flFront;

	float rotation = cmd->viewangles.y + 180;


	rotation = atan2( flXPosition, flYPosition ) + M_PI;
	rotation *= 180.0f / M_PI;

	float flYawRadians = -( rotation )* M_PI / 180.0f;
	float flCosYaw = cos( flYawRadians );
	float flSinYaw = sin( flYawRadians );

	flXPosition = ( int )( ( iScreenWidth / 2.0f ) + ( flRadius * flSinYaw ) );
	flYPosition = ( int )( ( iScreenHeight / 2.0f ) - ( flRadius * flCosYaw ) );

	if ( Source::WorldToScreen( vecDelta, vRealAngles ) )
		return;

	Source::m_pRenderer->DrawRect( flXPosition, flYPosition, 10, 10, Color::Green );
}

void Menu()
{
	Source::m_pMenu->OnPresentDevice();
}

void Watermark()
{
	/*int w, h;
	Source::m_pEngine->GetScreenSize( w, h );
	Source::m_pRenderer->DrawText( Source::m_hFont, ( float )w - 5.0f, 5.0f, FONT_ALIGN_RIGHT, Direct3D9::Color::White, XorStr( "%s" ), EdgeDetectMessage);*/
}

void __fastcall Hooked_PaintTraverse( void* ecx, void* edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce )
{
	auto pLocal = C_CSPlayer::GetLocalPlayer();
	Source::m_pPanelSwap->VCall< PaintTraverseFn >( 40 )( ecx, vguiPanel, forceRepaint, allowForce );
	if ( IsGoodPanel( vguiPanel ) )
	{
		//if ( !Source::m_pEngine->IsConnected() || !Source::m_pEngine->IsInGame() )
		//	return;

		//if ( Config::Render->NightModePower <= 100 )
		//{
		//	//NightMode();
		//}

		if ( Config::Render->NoSmoke || Config::Render->NoFlash || Config::Render->WireframeSmoke )
		{
			RemoveTexture();
		}

		//if ( Config::Render->AsusWallsPower <= 100 )
		//{
		//	//AsusWalls();
		//}

		//NightMode();

	}
}

void PresentProxy()
{
	__try
	{
		Source::m_pRenderer->Begin();

		if( !Shared::m_bPanic )
		{	
			ESP();
			Menu();
			Watermark();
		}

		Source::m_pRenderer->End();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		
	}
}

HRESULT D3DAPI Hooked_Present(IDirect3DDevice9* device, const RECT* source_rect, const RECT* dest_rect, HWND dest_window_override, const RGNDATA* dirty_region)
{
	PresentProxy();
	return Source::m_pDeviceSwap->VCall< PresentFn >(IDirect3DDevice9_Present)(device, source_rect, dest_rect, dest_window_override, dirty_region);
}


void Hooked_CL_RunPrediction( PREDICTION_REASON reason )
{
	static auto s_nSignonState = *( std::uintptr_t* )( Memory::PatternScan( XorStr( "engine.dll" ), XorStr( "83 3D ?? ?? ?? ?? ?? A3 ?? ?? ?? ?? 75 47" ) ) + 2 );
	static auto s_nDeltaTick = *( std::uintptr_t* )( Memory::PatternScan( XorStr( "engine.dll" ), XorStr( "83 3D ?? ?? ?? ?? ?? 7C 3E 8B 0D" ) ) + 2 );
	static auto s_last_command_ack = *( std::uintptr_t* )( Memory::PatternScan( XorStr( "engine.dll" ), XorStr( "A1 ?? ?? ?? ?? 56 50 A1" ) ) + 1 );
	static auto s_lastoutgoingcommand = *( std::uintptr_t* )( Memory::PatternScan( XorStr( "engine.dll" ), XorStr( "A1 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 8B 11 53 56" ) ) + 1 );
	static auto s_chokedcommands = *( std::uintptr_t* )( Memory::PatternScan( XorStr( "engine.dll" ), XorStr( "8B 35 ?? ?? ?? ?? 03 F0 A1 ?? ?? ?? ??" ) ) + 2 );

	int nSignonState = *( int* )s_nSignonState;
	int nDeltaTick = *( int* )s_nDeltaTick;
	int last_command_ack = *( int* )s_last_command_ack;
	int lastoutgoingcommand = *( int* )s_lastoutgoingcommand;
	int chokedcommands = *( int* )s_chokedcommands;

	if( !( nSignonState == 6 ) )
		return;

	if( nDeltaTick < 0 )
		return;

	bool valid = nDeltaTick > 0;

	Source::m_pPrediction->Update( nDeltaTick, valid, last_command_ack, lastoutgoingcommand + chokedcommands );
}

void DT_BasePlayer_m_nTickBase( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	*( int* )( pOut ) = pData->m_Value.m_Int;

	if( pStruct == C_CSPlayer::GetLocalPlayer() )
		Source::m_pDataManager->OnDataRecieved();
}

void DT_BasePlayer_m_vecPunchAngle( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	*( Vector3* )( pOut ) = pData->m_Value.m_Vector;
}

void DT_ParticleSmokeGrenade_m_flSpawnTime( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	float Value = pData->m_Value.m_Float;

	if( Config::Removals->NoSmoke )
		Value = 0.0f;

	*( float* )( pOut ) = Value;
}

void DT_CSPlayer_m_angEyeAnglesX( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	float angle = pData->m_Value.m_Float;
	player_info_t data;
	auto local = C_CSPlayer::GetLocalPlayer();
	auto player = ( C_CSPlayer* )pStruct;

	auto player_from_list = Source::m_pPlayerList->GetPlayer( player->GetIndex() );

	if ( Source::m_pEngine->GetPlayerInfo( player->GetIndex(), &data ) )
	{
		if ( data.fakeplayer == true )
			return;

		if ( player && Config::Misc->AutoFixPitch == true )
		{
			if ( angle > 181 && angle < 271 )
			{
				angle = 269.0;
				g_CorrectAngles = false;
				g_FixPitch = true;
			}

			else if ( angle > 89 && angle < 181 )
			{
				angle = 89.0;
				g_CorrectAngles = false;
				g_FixPitch = true;
			}

			else if ( angle < -89 )
			{
				angle = -89;
				g_CorrectAngles = false;
				g_FixPitch = true;
			}

			else if ( angle < 89 || angle > -89 )
			{
				g_CorrectAngles = true;
				g_FixPitch = false;
			}

			else
			{
				g_CorrectAngles = false;
				g_FixPitch = false;
			}
		}
		else if ( player && Config::Misc->AutoFixPitch == false )
		{
			g_CorrectAngles = false;
			g_FixPitch = false;
		}
	}
	*( float* )( pOut ) = angle;
}

void DT_CSPlayer_m_angEyeAnglesY( const CRecvProxyData* pData, void* pStruct, void* pOut )
{
	float angle = pData->m_Value.m_Float;
	static float changedvalue;
	int firedshots;
	auto local = C_CSPlayer::GetLocalPlayer();
	auto player = ( C_CSPlayer* )pStruct;
	auto player_from_list = Source::m_pPlayerList->GetPlayer( player->GetIndex() );
	static float fakeangle = 35;
	player_info_t data;

	if ( local && player )
	{
		if ( Config::Misc->Resolver == 1 && C_CSPlayer::GetLocalPlayer()->m_iShotsFired() >= Config::Misc->ShotsFired )
		{
			static float s = -360.0f;
			s = s + Config::Misc->CorrectioSpeedPower;
			if ( s > 360.0f )
				s = -360.0f;

			changedvalue = s;
			angle += changedvalue;
		}
		else if ( Config::Misc->Resolver == 2 && C_CSPlayer::GetLocalPlayer()->m_iShotsFired() >= Config::Misc->ShotsFired )
		{
			int speed = Config::Misc->CorrectioSpeedPower;
			static int spin = 0;
			spin += speed;
			changedvalue = spin;
			angle += changedvalue;
		}
		else if ( Config::Misc->Resolver == 3 )
		{
			if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 1 )
			{
				firedshots = 1;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 2 )
			{
				firedshots = 2;
			} 			
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 3 )
			{
				firedshots = 3;
			} 			
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 4 )
			{
				firedshots = 4;
			} 			
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 5 )
			{
				firedshots = 5;
			} 			
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 6 )
			{
				firedshots = 6;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 7 )
			{
				firedshots = 7;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 8 )
			{
				firedshots = 8;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 9 )
			{
				firedshots = 9;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() == 10 )
			{
				firedshots = 10;
			}
			else if ( C_CSPlayer::GetLocalPlayer()->m_iShotsFired() >= 11 )
			{
				firedshots = 11;
			}

			switch ( firedshots )
			{
				case 1:angle = angle + 180; break;
				case 2:angle = angle - 90; break;
				case 3:angle = angle + 90; break;
				case 4:angle = angle - 150; break;
				case 5:angle = angle + 150; break;
				case 6:angle = angle - 120; break;
				case 7:angle = angle + 120; break;
				case 8:angle = angle - 60; break;
				case 9:angle = angle + 60; break;
				case 10:angle = angle - 30; break;
				case 11:angle = angle + 30; break;
				case 12:angle = angle + 0; break;
			}
		}
	}
	*( float* )( pOut ) = angle;
}