#include "cbase.h"
#include "dp_teammenu.h"

#include "hud.h"
#include "iclientmode.h"
#include "c_baseplayer.h"
#include "hl2mp_gamerules.h"
#include "game_controls/basemodelpanel.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>

#include "tier0/memdbgon.h"

using namespace vgui;

static void SetFactionModel( CModelPanel *pPanel, const char *pszModel, int iSkin )
{
	KeyValues *pSettings = new KeyValues( "DPTeamModel" );
	pSettings->SetInt( "fov", 44 );
	pSettings->SetInt( "allow_offscreen", 1 );

	KeyValues *pModel = new KeyValues( "model" );
	pModel->SetString( "modelname", pszModel );
	pModel->SetInt( "skin", iSkin );
	pModel->SetInt( "spotlight", 1 );
	pModel->SetFloat( "angles_y", 180.0f );
	pModel->SetFloat( "origin_x", 46.0f );
	pModel->SetFloat( "origin_y", 0.0f );
	pModel->SetFloat( "origin_z", -64.0f );

	KeyValues *pAnim = new KeyValues( "animation" );
	pAnim->SetString( "name", "idle" );
	pAnim->SetString( "sequence", "BreathingIdle" );
	pAnim->SetInt( "default", 1 );
	pModel->AddSubKey( pAnim );

	pSettings->AddSubKey( pModel );
	pPanel->ApplySettings( pSettings );
	pSettings->deleteThis();
}

CDPTeamBox::CDPTeamBox( CDPTeamMenu *pParent, const char *pName, int iTeam, const char *pTitle ) : BaseClass( pParent, pName, "" )
{
	m_pTeamMenu = pParent;
	m_iTeam = iTeam;
	m_bHovered = false;
	m_bSelected = false;

	SetPaintBackgroundEnabled( false );
	SetPaintBorderEnabled( false );
	SetMouseInputEnabled( true );

	m_pTitle = new Label( this, "Title", pTitle );
	m_pTitle->SetContentAlignment( Label::a_center );
	m_pTitle->SetPaintBackgroundEnabled( false );
	m_pTitle->SetMouseInputEnabled( false );

	m_pModel = new CModelPanel( this, "FactionModel" );
	m_pModel->SetPaintBackgroundEnabled( false );
	m_pModel->SetMouseInputEnabled( false );
	m_pModel->SetKeyBoardInputEnabled( false );
	m_pModel->SetBgColor( Color( 0, 0, 0, 0 ) );

	if ( m_iTeam == TEAM_COMBINE )
	{
		SetFactionModel( m_pModel, "models/combine_soldier.mdl", 0 );
	}
	else
	{
		SetFactionModel( m_pModel, "models/humans/group03/male_07.mdl", 0 );
	}

	m_pTitle->MoveToFront();
}

void CDPTeamBox::SetSelected( bool bSelected )
{
	if ( m_bSelected == bSelected )
		return;

	m_bSelected = bSelected;
	Repaint();
}

void CDPTeamBox::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pTitle->SetFont( pScheme->GetFont( "ClientTitleFont", true ) );
	m_pTitle->SetFgColor( Color( 255, 255, 255, 255 ) );
}

void CDPTeamBox::PerformLayout( void )
{
	BaseClass::PerformLayout();

	int iWide, iTall;
	GetSize( iWide, iTall );

	const int iInset = 8;

	m_pModel->SetBounds( iInset, iInset, iWide - iInset * 2, iTall - iInset * 2 );
	m_pTitle->SetBounds( 8, iTall - 64, iWide - 16, 52 );
}

void CDPTeamBox::Paint( void )
{
	int iWide, iTall;
	GetSize( iWide, iTall );

	surface()->DrawSetColor( Color( 255, 255, 255, m_bHovered ? 22 : 8 ) );
	surface()->DrawFilledRect( 1, 1, iWide - 1, iTall - 1 );

	int iBorderAlpha = m_bHovered ? 255 : 190;
	surface()->DrawSetColor( Color( 255, 255, 255, iBorderAlpha ) );
	surface()->DrawOutlinedRect( 0, 0, iWide, iTall );
	surface()->DrawOutlinedRect( 1, 1, iWide - 1, iTall - 1 );

	if ( m_bSelected )
	{
		surface()->DrawSetColor( Color( 255, 255, 255, 255 ) );
		surface()->DrawFilledRect( 0, 0, iWide, 3 );
		surface()->DrawFilledRect( 0, iTall - 3, iWide, iTall );
		surface()->DrawFilledRect( 0, 0, 3, iTall );
		surface()->DrawFilledRect( iWide - 3, 0, iWide, iTall );
	}
}

void CDPTeamBox::OnCursorEntered( void )
{
	BaseClass::OnCursorEntered();
	m_bHovered = true;
	Repaint();
}

void CDPTeamBox::OnCursorExited( void )
{
	BaseClass::OnCursorExited();
	m_bHovered = false;
	Repaint();
}

void CDPTeamBox::OnMousePressed( MouseCode code )
{
	BaseClass::OnMousePressed( code );

	if ( code == MOUSE_LEFT && m_pTeamMenu )
	{
		m_pTeamMenu->SelectTeam( m_iTeam );
	}
}

CDPTeamMenu::CDPTeamMenu( IViewPort *pViewPort ) : Frame( NULL, PANEL_TEAM )
{
	m_pViewPort = pViewPort;

	SetScheme( "ClientScheme" );
	SetMoveable( false );
	SetSizeable( false );
	SetTitleBarVisible( false );
	SetProportional( false );
	SetPaintBackgroundEnabled( false );
	SetVisible( false );
	SetMouseInputEnabled( false );

	m_pBoxes[0] = new CDPTeamBox( this, "VGSBox", TEAM_COMBINE, "VGS" );
	m_pBoxes[1] = new CDPTeamBox( this, "FUMBox", TEAM_REBELS, "FUM" );
}

CDPTeamMenu::~CDPTeamMenu()
{
}

void CDPTeamMenu::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}

void CDPTeamMenu::PerformLayout( void )
{
	BaseClass::PerformLayout();

	int iWide, iTall;
	GetSize( iWide, iTall );

	int iBoxWide = MIN( 440, ( iWide - 160 ) / 2 );
	int iBoxTall = MIN( 620, iTall - 200 );
	iBoxWide = MAX( iBoxWide, 260 );
	iBoxTall = MAX( iBoxTall, 320 );

	const int iGap = 42;
	const int iStartX = ( iWide - ( iBoxWide * 2 + iGap ) ) / 2;
	const int iStartY = ( iTall - iBoxTall ) / 2;

	m_pBoxes[0]->SetBounds( iStartX, iStartY, iBoxWide, iBoxTall );
	m_pBoxes[1]->SetBounds( iStartX + iBoxWide + iGap, iStartY, iBoxWide, iBoxTall );
}

void CDPTeamMenu::Paint( void )
{
}

void CDPTeamMenu::OnThink( void )
{
	BaseClass::OnThink();

	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();
	int iLocalTeam = pLocalPlayer ? pLocalPlayer->GetTeamNumber() : TEAM_UNASSIGNED;

	for ( int i = 0; i < ARRAYSIZE( m_pBoxes ); ++i )
	{
		if ( m_pBoxes[i] )
			m_pBoxes[i]->SetSelected( iLocalTeam == ( i == 0 ? TEAM_COMBINE : TEAM_REBELS ) );
	}
}

void CDPTeamMenu::ShowPanel( bool bShow )
{
	if ( BaseClass::IsVisible() == bShow )
		return;

	if ( bShow )
	{
		int iWide, iTall;
		GetHudSize( iWide, iTall );
		SetBounds( 0, 0, iWide, iTall );
		InvalidateLayout( true, true );

		Activate();
		SetMouseInputEnabled( true );
		SetKeyBoardInputEnabled( true );

		if ( m_pBoxes[0] )
			m_pBoxes[0]->RequestFocus();

		m_pViewPort->ShowBackGround( false );
	}
	else
	{
		SetVisible( false );
		SetMouseInputEnabled( false );
		SetKeyBoardInputEnabled( false );
		m_pViewPort->ShowBackGround( false );
	}
}

void CDPTeamMenu::SelectTeam( int iTeam )
{
	char szCommand[32];
	Q_snprintf( szCommand, sizeof( szCommand ), "jointeam %d", iTeam );
	engine->ClientCmd( szCommand );

	ShowPanel( false );
}

void CDPTeamMenu::OnClose( void )
{
	BaseClass::OnClose();
	ShowPanel( false );
}

CON_COMMAND( dp_teammenu, "Opens the team selection menu" )
{
	if ( !gViewPortInterface )
		return;

	gViewPortInterface->ShowPanel( PANEL_TEAM, true );
}
