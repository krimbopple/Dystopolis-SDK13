#ifndef DP_TEAMMENU_H
#define DP_TEAMMENU_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include <game/client/iviewport.h>
#include "viewport_panel_names.h"

class CDPTeamMenu;
class CModelPanel;

class CDPTeamBox : public vgui::Button
{
	DECLARE_CLASS_SIMPLE( CDPTeamBox, vgui::Button );

public:
	CDPTeamBox( CDPTeamMenu *pParent, const char *pName, int iTeam, const char *pTitle );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout( void );
	virtual void Paint( void );
	virtual void OnCursorEntered( void );
	virtual void OnCursorExited( void );
	virtual void OnMousePressed( vgui::MouseCode code );

	void SetSelected( bool bSelected );

private:
	CDPTeamMenu *m_pTeamMenu;
	int m_iTeam;
	bool m_bHovered;
	bool m_bSelected;

	vgui::Label *m_pTitle;
	CModelPanel *m_pModel;
};

class CDPTeamMenu : public vgui::Frame, public IViewPortPanel
{
	DECLARE_CLASS_SIMPLE( CDPTeamMenu, vgui::Frame );

public:
	CDPTeamMenu( IViewPort *pViewPort );
	virtual ~CDPTeamMenu();

	virtual const char *GetName( void ) { return PANEL_TEAM; }
	virtual void SetData( KeyValues *data ) {}
	virtual void Reset( void ) {}
	virtual void Update( void ) {}
	virtual bool NeedsUpdate( void ) { return false; }
	virtual bool HasInputElements( void ) { return true; }
	virtual void ShowPanel( bool bShow );

	vgui::VPANEL GetVPanel( void ) { return BaseClass::GetVPanel(); }
	virtual bool IsVisible( void ) { return BaseClass::IsVisible(); }
	virtual void SetParent( vgui::VPANEL parent ) { BaseClass::SetParent( parent ); }

	virtual GameActionSet_t GetPreferredActionSet( void ) { return GAME_ACTION_SET_IN_GAME_HUD; }

	void SelectTeam( int iTeam );

protected:
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout( void );
	virtual void Paint( void );
	virtual void OnThink( void );
	virtual void OnClose( void );

private:
	IViewPort *m_pViewPort;
	CDPTeamBox *m_pBoxes[2];
};

#endif // DP_TEAMMENU_H
