#include "cbase.h"
#include "hud_round_timer.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "clientmode_hl2mpnormal.h"
#include "hl2mp_gamerules.h"
#include "c_team.h"
#include "cdll_util.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>

#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_HUDELEMENT( CHudRoundTimer );

CHudRoundTimer::CHudRoundTimer( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudRoundTimer" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );
	SetSize( ScreenWidth(), ScreenHeight() );
}

void CHudRoundTimer::Init( void )
{
}

void CHudRoundTimer::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetPaintBackgroundEnabled( false );
	SetSize( ScreenWidth(), ScreenHeight() );

	m_hTimerFont = pScheme->GetFont( "DPRoundTimerFont", true );
	m_hLabelFont = pScheme->GetFont( "DPRoundLabelFont", true );
}

bool CHudRoundTimer::ShouldDraw( void )
{
	return CHudElement::ShouldDraw() && HL2MPRules() != NULL;
}

void CHudRoundTimer::DrawCenteredText( vgui::HFont hFont, const wchar_t *pText, int y, Color color )
{
	if ( !pText || !pText[0] )
		return;

	vgui::surface()->DrawSetTextFont( hFont );
	vgui::surface()->DrawSetTextColor( color );

	int iWidth = UTIL_ComputeStringWidth( hFont, pText );
	vgui::surface()->DrawSetTextPos( ( ScreenWidth() - iWidth ) / 2, y );
	vgui::surface()->DrawUnicodeString( pText );
}

void CHudRoundTimer::Paint( void )
{
	C_HL2MPRules *pRules = HL2MPRules();
	if ( !pRules )
		return;

	int iState = pRules->GetDPRoundState();
	int iRound = pRules->GetDPRoundNumber();
	int iWinner = pRules->GetDPWinningTeam();
	float flRemaining = pRules->GetDPStateEndTime() - gpGlobals->curtime;

	char szTitle[128] = "";
	char szTimer[32] = "";
	char szScore[128] = "";
	Color color( 255, 255, 255, 230 );

	switch ( iState )
	{
	case DP_ROUND_WARMUP:
		Q_strncpy( szTitle, "WAITING FOR PLAYERS", sizeof( szTitle ) );
		color = Color( 200, 200, 200, 200 );
		break;

	case DP_ROUND_FREEZE:
		Q_snprintf( szTitle, sizeof( szTitle ), "ROUND %d  FREEZE", iRound );
		color = Color( 255, 190, 60, 255 );
		break;

	case DP_ROUND_LIVE:
		Q_snprintf( szTitle, sizeof( szTitle ), "ROUND %d", iRound );
		break;

	case DP_ROUND_END:
		if ( iWinner == TEAM_UNASSIGNED )
		{
			Q_snprintf( szTitle, sizeof( szTitle ), "ROUND %d  DRAW", iRound );
		}
		else
		{
			C_Team *pWinner = GetGlobalTeam( iWinner );
			Q_snprintf( szTitle, sizeof( szTitle ), "ROUND %d  %s WINS", iRound, pWinner ? pWinner->Get_Name() : "TEAM" );
		}
		color = Color( 255, 80, 80, 255 );
		break;

	case DP_ROUND_MATCH_END:
		if ( iWinner == TEAM_UNASSIGNED )
		{
			Q_strncpy( szTitle, "MATCH DRAW", sizeof( szTitle ) );
		}
		else
		{
			C_Team *pWinner = GetGlobalTeam( iWinner );
			Q_snprintf( szTitle, sizeof( szTitle ), "MATCH OVER  %s WINS", pWinner ? pWinner->Get_Name() : "TEAM" );
		}
		color = Color( 255, 80, 80, 255 );
		break;
	}

	if ( iState == DP_ROUND_FREEZE || iState == DP_ROUND_LIVE )
	{
		int iSeconds = (int)( flRemaining + 0.999f );
		if ( iSeconds < 0 )
			iSeconds = 0;

		Q_snprintf( szTimer, sizeof( szTimer ), "%d:%02d", iSeconds / 60, iSeconds % 60 );
	}

	C_Team *pCombine = GetGlobalTeam( TEAM_COMBINE );
	C_Team *pRebels = GetGlobalTeam( TEAM_REBELS );

	if ( pCombine && pRebels )
	{
		Q_snprintf( szScore, sizeof( szScore ), "%s %d  -  %d %s",
			pCombine->Get_Name(), pCombine->Get_Score(),
			pRebels->Get_Score(), pRebels->Get_Name() );
	}

	wchar_t wszTitle[128];
	wchar_t wszTimer[32];
	wchar_t wszScore[128];

	g_pVGuiLocalize->ConvertANSIToUnicode( szTitle, wszTitle, sizeof( wszTitle ) );
	g_pVGuiLocalize->ConvertANSIToUnicode( szTimer, wszTimer, sizeof( wszTimer ) );
	g_pVGuiLocalize->ConvertANSIToUnicode( szScore, wszScore, sizeof( wszScore ) );

	DrawCenteredText( m_hLabelFont, wszTitle, 24, color );

	if ( szTimer[0] )
	{
		DrawCenteredText( m_hTimerFont, wszTimer, 44, color );
	}

	DrawCenteredText( m_hLabelFont, wszScore, 127, Color( 230, 230, 230, 220 ) );
}
