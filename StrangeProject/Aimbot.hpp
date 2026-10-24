#pragma once

#include "Valve.hpp"

namespace Feature
{

	class CAimbot
	{
	public:
		CAimbot( void );

		bool			ExecuteAimbot( CUserCmd* pCmd );

	private:
		void			DropTarget( void );
		void			GetBestTarget( void );

		bool			Valid( int index );

		void			MakeVector( const Vector3& vIn, Vector3& vOut );
		float			GetFOV( const Vector3& viewangles, const Vector3& vStart, const Vector3& vEnd );

	private:
		CUserCmd*		m_pCmd;

		C_CSPlayer*		m_pLocal;
		C_WeaponCSBaseGun*	m_pWeapon;

		Vector3			m_vEnd;
		Vector3			delta;

		int				m_iBestIndex;
		float			m_flBestTarget;
	};
}