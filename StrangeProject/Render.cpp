#include "Render.hpp"
#include "Source.hpp"
#include "Player.hpp"
#include "Config.hpp"

namespace Feature
{
	bool ChamsEnemyOnly = true;
	bool bChamsColored = true;
	Render::Render()
	{
		m_pVertexInModel = CreateMaterial( true, true );
		m_pVertexOutModel = CreateMaterial( true, false );		
		m_pVertexInWeapon = CreateMaterial( true, true );
		m_pVertexOutWeapon = CreateMaterial( true, false );
		m_pMatIn = CreateMaterial( false, true );
		m_pMatOut = CreateMaterial( false, false );
		m_pOut = CreateMaterial( false, true, false );
	}

	void Render::OnDrawModel( void* ecx, ModelRenderInfo_t* info )
	{
		std::string pszModelName = Source::m_pModelInfoClient->GetModelName( info->pModel );
		auto player = C_CSPlayer::GetLocalPlayer();
		auto enemy = ToCSPlayer( Source::m_pEntList->GetBaseEntity( info->ent_index ) );
		Color ModelsColor;
		Color WeaponsColor;
		IMaterial* pInModel = nullptr;
		IMaterial* pOutModel = nullptr;			
		IMaterial* pInWeapon = nullptr;
		IMaterial* pOutWeapon = nullptr;

		if ( std::string( pszModelName ).find( "models/player" ) != std::string::npos ) // Shadow
		{
			if ( enemy->m_iTeamNum() == 2 ) // T
			{
				ModelsColor = Color( 255, 0, 0, 255 );
			}

			else if ( enemy->m_iTeamNum() == 3 ) // CT
			{
				ModelsColor = Color( 0, 0, 255, 255 );
			}

			if ( enemy->m_lifeState() != LIFE_ALIVE )
				return;

			if ( ChamsEnemyOnly == true ) // Enemy
			{
				if ( player->m_iTeamNum() == enemy->m_iTeamNum() )
					return;
			}
			if ( std::string( pszModelName ).find( "models/player/ct_" ) != std::string::npos )
			{
				if ( Config::Render->ChamsActive )
				{
					if ( Config::Render->out )
						pInModel = nullptr;
					else
						pInModel = m_pVertexInModel;

					pOutModel = m_pVertexOutModel;
				}
			}

			if ( std::string( pszModelName ).find( "models/player/t_" ) != std::string::npos )
			{
				if ( Config::Render->ChamsActive )
				{
					if ( Config::Render->out )
						pInModel = nullptr;
					else
						pInModel = m_pVertexInModel;

					pOutModel = m_pVertexOutModel;
				}
			}

			if ( Config::Render->Wireframe )
			{
				pInModel->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, true );
				pOutModel->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, true );
			}
			else
			{
				pInModel->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, false );
				pOutModel->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, false );
			}
			ForceMaterial( ModelsColor, pInModel );
			Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
			ForceMaterial( ModelsColor, pOutModel );
		}
		else if ( std::string( pszModelName ).find( "models/weapons/v_" ) != std::string::npos )
		{
			WeaponsColor = Color( 70, 70, 70, 255);
			if ( Config::Render->HandChams )
			{
				if ( Config::Render->out )
					pInWeapon = nullptr;
				else
					pInWeapon = m_pVertexInWeapon;

				pOutWeapon = m_pVertexOutWeapon;
			}

			pOutWeapon->SetMaterialVarFlag( MATERIAL_VAR_TRANSLUCENT, true );
			pInWeapon->SetMaterialVarFlag( MATERIAL_VAR_TRANSLUCENT, true );
			ForceMaterial( WeaponsColor, pInWeapon );
			Source::m_pModelRenderSwap->VCall< DrawModelExFn >( IVModelRender_DrawModelEx )( ecx, info );
			ForceMaterial( WeaponsColor, pOutWeapon );
		}
	}

	IMaterial* Render::CreateMaterial( bool bVertexLit, bool bIgnoreZ, bool bWireframe /*= false*/ )
	{
		static int iCreated = 0;
		static const char szMaterialStruct[ ] =
		{
			"\"%s\"\
			\n{\
			\n\t\"$basetexture\" \"vgui/white_additive\"\
			\n\t\"$envmap\" \"\"\
			\n\t\"$model\" \"1\"\
			\n\t\"$receiveflashlight\" \"1\"\
			\n\t\"$singlepassflashlight\" \"1\"\
			\n\t\"$flat\" \"1\"\
			\n\t\"$nocull\" \"0\"\
			\n\t\"$selfillum\" \"1\"\
			\n\t\"$halflambert\" \"1\"\
			\n\t\"$nofog\" \"0\"\
			\n\t\"$ignorez\" \"%i\"\
			\n\t\"$znearer\" \"0\"\
			\n\t\"$wireframe\" \"%i\"\
			\n}\n"
		};

		const char* szBaseType = bVertexLit ? "VertexLitGeneric" : "UnlitGeneric";

		char szMaterial[ 512 ];
		sprintf_s( szMaterial, sizeof( szMaterial ), szMaterialStruct, szBaseType, bIgnoreZ ? 1 : 0, bWireframe ? 1 : 0 );

		char szName[ 512 ];
		sprintf_s( szName, sizeof( szName ), "custom_material_%i.vmt", iCreated );

		iCreated++;

		KeyValues* pKey = new KeyValues( szBaseType );

		pKey->LoadFromBuffer( szName, szMaterial );

		IMaterial* pMat = Source::m_pMatSystem->CreateMaterial( szName, pKey );

		return pMat;
	}

	void Render::ForceMaterial( const Color& color, IMaterial* mat, bool mod )
	{
		if( mod )
		{
			float col[ 3 ] =
			{
				color.R / 255.0f,
				color.G / 255.0f,
				color.B / 255.0f
			};

			float alpha = color.A / 255.0f;

			Source::m_pRenderView->SetBlend( alpha );
			Source::m_pRenderView->SetColorModulation( col );
		}

		Source::m_pModelRender->ForcedMaterialOverride( mat );
	}
}