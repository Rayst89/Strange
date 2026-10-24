#include "Aimbot.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Weapon.hpp"
#include "Config.hpp"
#include <stdlib.h>

namespace Feature
{
	CAimbot::CAimbot( void )
	{
		m_pCmd = nullptr;

		m_pLocal = nullptr;
		m_pWeapon = nullptr;

		m_iBestIndex = -1;
		m_flBestTarget = 0.0f;
	}
#define DEFAULT_TICK_INTERVAL	(0.015)	
#define TICK_INTERVAL			DEFAULT_TICK_INTERVAL
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
	float LagFix1()
	{
		float updaterate = Source::m_pCvar->FindVar( "cl_updaterate" )->GetFloat();
		ConVar* minupdate = Source::m_pCvar->FindVar( "sv_minupdaterate" );
		ConVar* maxupdate = Source::m_pCvar->FindVar( "sv_maxupdaterate" );

		if ( minupdate && maxupdate )
			updaterate = maxupdate->GetFloat();

		float ratio = Source::m_pCvar->FindVar( "cl_interp_ratio" )->GetFloat();

		if ( ratio == 0 )
			ratio = 1.0f;

		float lerp = Source::m_pCvar->FindVar( "cl_interp" )->GetFloat();
		ConVar* cmin = Source::m_pCvar->FindVar( "sv_client_min_interp_ratio" );
		ConVar* cmax = Source::m_pCvar->FindVar( "sv_client_max_interp_ratio" );

		if ( cmin && cmax && cmin->GetFloat() != 1 )
			ratio = clamp( ratio, cmin->GetFloat(), cmax->GetFloat() );

		return max( lerp, ratio / updaterate );
	}

	float LagFix2()
	{
		static ConVar* cl_interp_ratio = Source::m_pCvar->FindVar( XorStr( "cl_interp_ratio" ) );
		static ConVar* cl_updaterate = Source::m_pCvar->FindVar( XorStr( "cl_updaterate" ) );
		static ConVar* sv_client_min_interp_ratio = Source::m_pCvar->FindVar( XorStr( "sv_client_min_interp_ratio" ) );
		static ConVar* sv_client_max_interp_ratio = Source::m_pCvar->FindVar( XorStr( "sv_client_max_interp_ratio" ) );
		static ConVar* sv_minupdaterate = Source::m_pCvar->FindVar( XorStr( "sv_minupdaterate" ) );
		static ConVar* sv_maxupdaterate = Source::m_pCvar->FindVar( XorStr( "sv_maxupdaterate" ) );
		static ConVar* cl_interp = Source::m_pCvar->FindVar( XorStr( "cl_interp" ) );
		static ConVar* cl_interpolate = Source::m_pCvar->FindVar( XorStr( "cl_interpolate" ) );




		//UpdateRate
		float interp = cl_interp->GetFloat();
		float flUpdateRate = cl_updaterate->GetFloat();
		float  interp_ratio = cl_interp_ratio->GetFloat();

		if ( sv_client_min_interp_ratio->GetFloat() > interp_ratio )
			interp_ratio = sv_client_min_interp_ratio->GetFloat();
		if ( interp_ratio > sv_client_max_interp_ratio->GetFloat() )
			interp_ratio = sv_client_max_interp_ratio->GetFloat();
		if ( sv_maxupdaterate->GetFloat() <= flUpdateRate )
			flUpdateRate = sv_maxupdaterate->GetFloat();
		if ( sv_minupdaterate->GetFloat() > flUpdateRate )
			flUpdateRate = sv_minupdaterate->GetFloat();

		float v4 = interp_ratio / flUpdateRate;
		if ( v4 > interp )
			interp = v4;


		//I really hope this was a macro which is compiler inserted, otherwise shame on aw
		return ( ( ( 1.0 / Source::m_pGlobalVars->interval_per_tick ) * 90000000000 ) + 0.5 ) //TIME_TO_TICKS SHIT
			+ ( ( ( 1.0 / Source::m_pGlobalVars->interval_per_tick ) * interp ) + 0.5 );
	}

	float InterpFix()
	{
		static ConVar* cvar_cl_interp = Source::m_pCvar->FindVar( "cl_interp" );
		static ConVar* cvar_cl_updaterate = Source::m_pCvar->FindVar( "cl_updaterate" );
		static ConVar* cvar_sv_maxupdaterate = Source::m_pCvar->FindVar( "sv_maxupdaterate" );
		static ConVar* cvar_sv_minupdaterate = Source::m_pCvar->FindVar( "sv_minupdaterate" );
		static ConVar* cvar_cl_interp_ratio = Source::m_pCvar->FindVar( "cl_interp_ratio" );

		float cl_interp = cvar_cl_interp->GetFloat();
		int cl_updaterate = cvar_cl_updaterate->GetInt();
		int sv_maxupdaterate = cvar_sv_maxupdaterate->GetInt();
		int sv_minupdaterate = cvar_sv_minupdaterate->GetInt();
		int cl_interp_ratio = cvar_cl_interp_ratio->GetInt();

		if ( sv_maxupdaterate <= cl_updaterate )
			cl_updaterate = sv_maxupdaterate;

		if ( sv_minupdaterate > cl_updaterate )
			cl_updaterate = sv_minupdaterate;

		float new_interp = ( float )cl_interp_ratio / ( float )cl_updaterate;

		if ( new_interp > cl_interp )
			cl_interp = new_interp;

		return max( cl_interp, cl_interp_ratio / cl_updaterate );
	}

	bool CAimbot::ExecuteAimbot( CUserCmd* pCmd )
	{
		if ( !Config::Current->Aimbot->Active )
			return false;

		m_pCmd = pCmd;

		if ( !m_pCmd )
			return false;

		m_pLocal = C_CSPlayer::GetLocalPlayer();

		if ( !m_pLocal )
			return false;

		m_pWeapon = m_pLocal->GetActiveWeapon();

		if ( !m_pWeapon )
			return false;

		if ( m_iBestIndex == -1 )
			GetBestTarget();

		DropTarget();

		if ( m_iBestIndex != -1 )
		{
			Vector3 vDirection = m_vEnd - m_pLocal->EyePosition();
			Vector3 vAim;

			VectorNormalize( vDirection );

			VectorAngles( vDirection, vAim );

			AnglesNormalize( vAim );
			vAim.z = 0.0f;

			VectorCopy( vAim, m_pCmd->viewangles );

			if ( !Config::Current->Aimbot->Silent )
				Source::m_pEngine->SetViewAngles( m_pCmd->viewangles );

			if ( Config::Current->Aimbot->AutoFire )
			{
				if ( !m_pWeapon->m_iClip1() )
				{
					m_pCmd->buttons |= IN_RELOAD;
					m_pCmd->buttons &= ~IN_ATTACK;
				}
				else
				{
					m_pCmd->buttons &= ~IN_RELOAD;
					m_pCmd->buttons |= IN_ATTACK;
				}
			}

			return true;
		}

		return false;
	}

	void CAimbot::DropTarget()
	{
		if ( !Valid( m_iBestIndex ) )
			m_iBestIndex = -1;
	}

	void CAimbot::GetBestTarget()
	{
		m_flBestTarget = 360;

		auto nSize = Source::m_pEngine->GetMaxClients();

		for ( auto i = 1; i <= nSize; i++ )
		{
			if ( Valid( i ) )
			{
				float flFOV = GetFOV( m_pCmd->viewangles, m_pLocal->EyePosition(), m_vEnd );

				if ( flFOV < m_flBestTarget )
				{
					m_flBestTarget = flFOV;
					m_iBestIndex = i;
				}
			}
		}
	}

	bool CAimbot::Valid( int index )
	{
		auto pEntity = Source::m_pEntList->GetBaseEntity( index );

		if ( !pEntity )
			return false;

		C_CSPlayer* pPlayer = ToCSPlayer( pEntity );

		if ( !pPlayer )
			return false;

		if ( pPlayer->IsDormant() )
			return false;

		if ( pPlayer->m_lifeState() != LIFE_ALIVE )
			return false;

		if ( pPlayer->m_iHealth() > 500 )
			return false;

		if ( pPlayer->m_iTeamNum() == m_pLocal->m_iTeamNum() )
			return false;

		if ( !pPlayer->GetHitboxVector( Config::Current->Aimbot->Spot, m_vEnd ) )
			return false;

		if ( Config::Current->Aimbot->AutoWall )
		{
			if ( Config::Current->Aimbot->HitScan == 1 )
			{
				if ( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vEnd, Config::Current->Aimbot->MinDamage ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pPlayer->GetModel() )->GetHitboxSet( pPlayer->m_nHitboxSet() );

					int iBestDamage = Config::Current->Aimbot->MinDamage;

					bool bSuccess = false;

					for ( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vCurrent;

						if ( !pPlayer->GetHitboxVector( i, vCurrent ) )
							continue;

						int iDamage = -1;

						Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vCurrent, Config::Current->Aimbot->MinDamage, 0, &iDamage );

						if ( iDamage >= iBestDamage )
						{
							iBestDamage = iDamage;
							m_vEnd = vCurrent;
							bSuccess = true;
						}
					}

					if ( !bSuccess )
						return false;
				}
			}
			else if ( Config::Current->Aimbot->HitScan == 2 )
			{
				if ( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vEnd, Config::Current->Aimbot->MinDamage ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pPlayer->GetModel() )->GetHitboxSet( pPlayer->m_nHitboxSet() );

					int iBestDamage = Config::Current->Aimbot->MinDamage;

					bool bSuccess = false;

					for ( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vMin, vMax;

						if ( !pPlayer->GetHitboxBounds( i, vMin, vMax ) )
							continue;

						vMin *= Config::Current->Aimbot->HitScanScale;
						vMax *= Config::Current->Aimbot->HitScanScale;

						int iDamage = -1;

						if ( Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vMin, Config::Current->Aimbot->MinDamage, 0, &iDamage ) )
						{
							if ( iDamage >= iBestDamage )
							{
								iBestDamage = iDamage;
								m_vEnd = vMin;
								bSuccess = true;
							}
						}
						else if ( Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), vMax, Config::Current->Aimbot->MinDamage, 0, &iDamage ) )
						{
							if ( iDamage >= iBestDamage )
							{
								iBestDamage = iDamage;
								m_vEnd = vMax;
								bSuccess = true;
							}
						}
					}

					if ( !bSuccess )
						return false;
				}
			}
			else
			{
				if ( !Source::m_pAccuracy->CanPenetrate( m_pLocal->EyePosition(), m_vEnd, Config::Current->Aimbot->MinDamage ) )
					return false;
			}
		}
		else
		{
			if ( Config::Current->Aimbot->HitScan == 1 )
			{
				if ( !Source::TraceLine( m_vEnd, pPlayer ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pPlayer->GetModel() )->GetHitboxSet( pPlayer->m_nHitboxSet() );

					bool bSuccess = false;

					for ( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vCurrent;

						if ( !pPlayer->GetHitboxVector( i, vCurrent ) )
							continue;

						if ( !Source::TraceLine( vCurrent ) )
							continue;

						bSuccess = true;

						m_vEnd = vCurrent;
					}

					if ( !bSuccess )
						return false;
				}
			}
			else if ( Config::Current->Aimbot->HitScan == 2 )
			{
				if ( !Source::TraceLine( m_vEnd, pPlayer ) )
				{
					auto pSet = Source::m_pModelInfoClient->GetStudioModel( pPlayer->GetModel() )->GetHitboxSet( pPlayer->m_nHitboxSet() );

					bool bSuccess = false;

					for ( int i = 0; i < pSet->numhitboxes; i++ )
					{
						Vector3 vMin, vMax;

						if ( !pPlayer->GetHitboxBounds( i, vMin, vMax ) )
							continue;

						vMin *= Config::Current->Aimbot->HitScanScale;
						vMax *= Config::Current->Aimbot->HitScanScale;

						if ( Source::TraceLine( vMin ) )
						{
							bSuccess = true;

							m_vEnd = vMin;
						}
						else if ( Source::TraceLine( vMax ) )
						{
							bSuccess = true;

							m_vEnd = vMax;
						}
					}

					if ( !bSuccess )
						return false;
				}
			}
			else
			{
				if ( !Source::TraceLine( m_vEnd, pPlayer ) )
					return false;
			}
		}


		auto distance = m_vEnd.DistTo( m_pLocal->EyePosition() );
		float latency = Source::m_pEngine->GetNetChannelInfo()->GetLatency( 0 );
		delta[ 0 ] *= latency;
		delta[ 1 ] *= latency;
		delta[ 2 ] *= latency;
		m_vEnd += pPlayer->m_vecVelocity() / distance;
		m_vEnd -= m_pLocal->m_vecVelocity() / distance;
		m_vEnd += pPlayer->m_vecVelocity() * Source::m_pGlobalVars->interval_per_tick;
		if ( Config::AntiAim->LagFixWithAim == 1 )
		{
			m_pCmd->tick_count = TIME_TO_TICKS( pPlayer->GetSimulationTime() + LagFix1() );
		}

		if ( Config::AntiAim->LagFixWithAim == 2 )
		{
			m_pCmd->tick_count = TIME_TO_TICKS( pPlayer->GetSimulationTime() + LagFix2() );
		}

		if ( Config::AntiAim->LagFixWithAim == 3 )
		{
			m_pCmd->tick_count = TIME_TO_TICKS( InterpFix() );
		}

		return true;
	}

	void CAimbot::MakeVector( const Vector3& vIn, Vector3& vOut )
	{
		float pitch = ToRadians( vIn.x );
		float yaw = ToRadians( vIn.y );
		float temp = cos( pitch );

		vOut.x = -temp * -cos( yaw );
		vOut.y = sin( yaw ) * temp;
		vOut.z = -sin( pitch );
	}

	float CAimbot::GetFOV( const Vector3& viewangles, const Vector3& vStart, const Vector3& vEnd )
	{
		Vector3 vAng, vAim;

		Vector3 vDir = vEnd - vStart;

		VectorNormalize( vDir );

		VectorAngles( vDir, vAng );

		MakeVector( viewangles, vAim );
		MakeVector( vAng, vAng );

		return ToDegrees( acos( vAim.Dot( vAng ) ) / vAim.LengthSqr() );
	}
}