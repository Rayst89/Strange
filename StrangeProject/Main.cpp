#include "Source.hpp"
#include "Config.hpp"

#include "ImGui/imgui.h"
#include <iostream>
#include <Windows.h>
#include <playsoundapi.h>
#pragma comment(lib, "Winmm.lib")
void Eject()
{
	if( !Source::Release() )
		printf_s( XorStr( "Unload Can't release 'Source' hooks!" ) );

	Sleep( 1000 );
	Source::Free();
	Config::Release();
}

void CreateConsole( std::string Title )
{
	AllocConsole();
	freopen( "CONIN$", "r", stdin );
	freopen( "CONOUT$", "w", stdout );
	freopen( "CONOUT$", "w", stderr );

	SetConsoleTitle( Title.c_str() );
}

void Heart( HMODULE hMod )
{
	Config::Startup( hMod );

	if( !Source::Startup() )
	{
		printf_s( XorStr( "[Heart] Can't initialize 'Source' hooks! Ejecting!" ) );
		Eject();
		FreeLibraryAndExitThread( hMod, EXIT_SUCCESS );
	}


	while( !Shared::m_bEject )
	{
		if( Shared::m_bLoad )
		{
			Config::Load( Shared::m_strConfig );
			Shared::m_bLoad = false;
		}

		if( Shared::m_bSave )
		{
			Config::Save( Shared::m_strConfig );
			Shared::m_bSave = false;
		}

		Sleep( 100 );
	}

	Eject();
	FreeLibraryAndExitThread( hMod, EXIT_SUCCESS );
}

static Shared::Vars vars =
{
	"C:\\"
};

VOID CheckValidHardwareID()
{
	char password[ 32 ];
#define My_HWID (1724829454) // 1724829454
#define Sand_Jar (879332655) // 879332655
	UCHAR szFileSys[ 255 ], szVolNameBuff[ 255 ];
	DWORD dwMFL, dwSysFlags, dwSerial;
	GetVolumeInformationA( "C:\\", ( LPSTR )szVolNameBuff, 255, &dwSerial, &dwMFL, &dwSysFlags, ( LPSTR )szFileSys, 255 );
	for ( int i = 0; i < 32; i++ )
		password[ i ] = ' ';


	if ( dwSerial == My_HWID || dwSerial == Sand_Jar )
	{

	}
	else
	{
		ExitProcess( 1 );
	}
}

void OnProcessAttach( HMODULE hMod, LPVOID lpReserved )
{
	Shared::m_pVars = &vars;

	DisableThreadLibraryCalls( hMod );
	CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )Heart, ( LPVOID )hMod, NULL, NULL );
}

void OnProcessDetach()
{
	Eject();
}

BOOL WINAPI DllMain( HMODULE hMod, DWORD dwReason, LPVOID lpReserved )
{
	switch( dwReason )
	{
		case DLL_PROCESS_ATTACH:
		{
			//CreateConsole("DEBUG");
			//CheckValidHardwareID();
			OnProcessAttach( hMod, lpReserved );
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			OnProcessDetach();
			break;
		}
	}

	return TRUE;
}