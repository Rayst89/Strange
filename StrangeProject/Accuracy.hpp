#pragma once

#include "Valve.hpp"

namespace Feature
{
	class Accuracy
	{
	public:
		void				ApplySpreadFix( C_WeaponCSBaseGun* weapon, int random_seed, const Vector3& input, Vector3& va, int type = 2, bool inverted = false );
		void				ApplyRecoilFix( C_CSPlayer* player, Vector3& va, bool inverted = false );

		void				NoRecoil( CUserCmd * cmd, C_CSPlayer * player, C_WeaponCSBaseGun * weapon );
		void				NoSpread( CUserCmd * cmd, C_WeaponCSBaseGun * weapon );

		bool				CanPenetrate( const Vector3& vStart, const Vector3& vEnd, int iMinDamage, int iTeam = 0, int* pDamageOut = nullptr, int* pHitbox = nullptr, int* pHitgroup = nullptr, C_BaseEntity** ppEnt = nullptr );
	};
}