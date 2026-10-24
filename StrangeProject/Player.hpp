#pragma once

#include "Entity.hpp"

enum class ObserverMode_t : int
{
	OBS_MODE_NONE = 0,
	OBS_MODE_DEATHCAM = 1,
	OBS_MODE_FREEZECAM = 2,
	OBS_MODE_FIXED = 3,
	OBS_MODE_IN_EYE = 4,
	OBS_MODE_CHASE = 5,
	OBS_MODE_ROAMING = 6
};

class C_CSPlayer : public C_BaseEntity
{
public:
	static C_CSPlayer*	GetLocalPlayer();

	// DT_BaseCombatCharacter
	CBaseHandle&		m_hActiveWeapon();
	CBaseHandle&        m_hObserverTarget();
	float&				m_flNextAttack();

	// DT_BaseAnimating
	int&				m_nHitboxSet();

	// DT_BasePlayer
	Vector3&			m_vecPunchAngle();
	Vector3&			m_vecViewOffset();
	float&				m_flFallVelocity();
	float&				m_iPing();
	Vector3&			m_vecVelocity();
	Vector3&			m_vecBaseVelocity();
	char&				m_lifeState();
	bool               m_bIsLife(C_CSPlayer * player);
	int&				m_nTickBase();
	int&				m_iHealth();
	int&				m_fFlags();
	int&				m_iFOV();
	DWORD				deadflag();

	// DT_CSPlayer
	Vector3&			m_angEyeAngles();

	bool&				m_bIsDefusing();
	bool				m_bInBuyZone();
	bool&				m_fCameraInThirdPerson();
	bool&				m_bHasHelmet();
	

	int&                m_iObserverMode();

	ObserverMode_t      GetObserverMode(C_CSPlayer * player);

	PVOID               GetObserverTarget(C_CSPlayer * player);

	int&				m_iShotsFired();
	int&				m_ArmorValue();

	float&				m_flStamina();
	float&				m_flFlashMaxAlpha();
	float&				m_flFlashDuration();

	void				SetCurrentCommand( CUserCmd* pCmd );

	

	// Custom
	C_WeaponCSBaseGun*	GetActiveWeapon();

	Vector3				EyePosition();
	Vector3				EyeAngles();

	bool				GetBoneCache( Matrix3x4* out );

	bool				GetBoneVector( int bone, Vector3& out );
	bool				GetHitboxVector( int hitbox, Vector3& out );
	bool				GetHitboxBounds( int hitbox, Vector3& mins, Vector3& maxs );
};

inline C_CSPlayer* ToCSPlayer( C_BaseEntity* pEnt )
{
	if( !pEnt )
		return nullptr;

	if( !pEnt->IsPlayer() )
		return nullptr;

	return ( C_CSPlayer* )pEnt;
}