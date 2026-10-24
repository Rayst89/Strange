#include "IGameEvent.h"
#include <Windows.h>
#include <playsoundapi.h>
#pragma comment(lib, "Winmm.lib")
#include "HitMarkerData.h"
std::vector< char* > m_words_array = { 
	"<Hey, guys, what's up?>",
	"[StrangeH00K]",
	"You can't hit me in the head? Pakistani cunt!",
	"Ultr@hook? xdd...",
	"Who is Rayst, you know?",
	"Can you tell me? How do you play with asmodeus? xdd",
	"Finding real head",
	"Did you pay?",
	"aka user EdeopH00K",
	"Don't talk to me.",
	"Admins -> petuxes",
	"If you want to cry... pls leave the game :)",
	"You have a shit aimbot! Download a new hack",
	"Are you a coder?",
	"999 + 1 = 1000, 999 + hook = shit",
	"If you want to download my hack then go to www.pornhub.org/gayporn",
	" ",
	"press F to pay respect",
	"Your english is so awesome.",
	"fuck, fuck, fuck... MaxuMuM is calling me",
	"1fy0uc4nr34dth1sy0u4r3g4y",
	"Can you send your HWID?",
	"Edeop[RIP]",
	"Where is you brain? :/",
	"#include <brain> //you need this",
	"Sand_Jar is my best friend!",
	"Do you aim at me?",
	"I put a bomb on your head",
	"My eng is poor",
	"Leave me alone",
	"I bought the StrangeH00K",
	"Sieg Heil!",
	"Sorry :)",
	"[+1 234 147-19-00] Altron needs help!! He has a HIV!",
	"[You] ______|_ [Me]",
	"turkey.core"
};
void cGameEventManager::FireGameEvent( IGameEvent *event )
{
	char str[ 128 ];
	const char* szEventName = event->GetName();

	if ( !szEventName )
		return;

	if ( szEventName )
	{
		if ( Config::Misc->SpamInChat == 2 )
		{
			if ( !strcmp( szEventName, "round_start" ) )
			{
				static auto index = 0;
				static auto size = m_words_array.size();

				if ( index >= size )
					index = 0;

				Source::SayInChat( m_words_array[ index ] );

				index += 1;
			}
		}

		if ( Config::Misc->RoundSay )
		{
			if ( !strcmp( szEventName, "round_start" ) )
			{
				if ( Config::Misc->RoundStartSay[ 0 ] != 0 )
				{
					Source::SayInChat( Config::Misc->RoundStartSay );
				}
			}

			if ( !strcmp( szEventName, "round_end" ) )
			{
				if ( Config::Misc->RoundEndSay[ 0 ] != 0 )
				{
					Source::SayInChat( Config::Misc->RoundEndSay );
				}
			}
		}

		if ( !strcmp( szEventName, "player_hurt" ) )
		{
			if ( Config::Misc->HitSound )
			{
				C_BaseEntity * attacker = Source::m_pEntList->GetBaseEntity( Source::m_pEngine->GetPlayerForUserID( event->GetInt( XorStr( "attacker" ) ) ) );
				C_BaseEntity * userid = Source::m_pEntList->GetBaseEntity( Source::m_pEngine->GetPlayerForUserID( event->GetInt( XorStr( "userid" ) ) ) );
				if ( attacker == C_CSPlayer::GetLocalPlayer() )
				{
					if ( userid->m_iTeamNum() == C_CSPlayer::GetLocalPlayer()->m_iTeamNum() )
						return;

					PlaySoundA( hitmarkerdata, NULL, 0x00002000L | 0x0001 | 0x0004 );
				}
			}
		}
	}
}

void cGameEventManager::RegisterSelf()
{
	Source::m_pGameEventManager2->AddListener( this, "round_start", false );
	Source::m_pGameEventManager2->AddListener( this, "round_end", false );
	Source::m_pGameEventManager2->AddListener( this, "player_hurt", false );
}

void cGameEventManager::RemoveListeners()
{
	Source::m_pGameEventManager2->RemoveListener( this );
	Source::m_pGameEventManager2->RemoveListener( this );
	Source::m_pGameEventManager2->RemoveListener( this );
}
