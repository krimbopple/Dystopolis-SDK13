#ifndef HUD_ROUND_TIMER_H
#define HUD_ROUND_TIMER_H
#pragma once

#include "hudelement.h"
#include <vgui_controls/Panel.h>
#include "Color.h"

class CHudRoundTimer : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudRoundTimer, vgui::Panel );
public:
	CHudRoundTimer( const char *pElementName );

	virtual void Init( void );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool ShouldDraw( void );
	virtual void Paint( void );

private:
	void DrawCenteredText( vgui::HFont hFont, const wchar_t *pText, int y, Color color );

	vgui::HFont m_hTimerFont;
	vgui::HFont m_hLabelFont;
};

#endif
