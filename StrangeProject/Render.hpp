#pragma once

#include "Valve.hpp"

namespace Feature
{
	using Direct3D9::Color;

	class Render
	{
	public:
		Render();

		void		OnDrawModel( void* ecx, ModelRenderInfo_t* info );

		IMaterial*	CreateMaterial( bool bVertexLit, bool bIgnoreZ, bool bWireframe = false );
		void		ForceMaterial( const Color& color, IMaterial* mat, bool mod = true );

	private:
		IMaterial*	m_pVertexInModel;
		IMaterial*	m_pVertexOutModel;		
		IMaterial*	m_pVertexInWeapon;
		IMaterial*	m_pVertexOutWeapon;

		IMaterial*	m_pMatIn;
		IMaterial*	m_pMatOut;

		IMaterial*	m_pOut;
	};
}