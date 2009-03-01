/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) 2003-2008 Thomas Giesel
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Thomas Giesel skoe@directbox.com
 */

#include <wx/mdi.h>
#include <wx/dc.h>
#include <wx/msgdlg.h>

#include "MCApp.h"
#include "MCDoc.h"
#include "MCCanvas.h"
#include "MCMainFrame.h"
#include "MCChildFrame.h"

MCChildFrame::MCChildFrame(MCDoc* pDoc, wxMDIParentFrame* pParent, wxWindowID id, const wxString& title) :
    wxMDIChildFrame(pParent, id, title),
    m_pDoc(pDoc),
    m_nScale(1),
    m_bTVMode(true),
    m_bMousePosDrawn(false)
{
    m_pCanvas = new MCCanvas(this, 0);

    m_pCanvas->SetDoc(pDoc);
    m_pCanvas->SetVirtualSize(MC_X * 2 * m_nScale, MC_Y * m_nScale);
    m_pCanvas->SetScrollRate(10 * m_nScale, 10 * m_nScale);
    m_pCanvas->SetScale(m_nScale);
    m_pCanvas->SetEmulateTV(m_bTVMode);

    Connect(wxEVT_ACTIVATE, wxActivateEventHandler(MCChildFrame::OnActivate));
}

/*****************************************************************************/
MCChildFrame::~MCChildFrame()
{
}


/*****************************************************************************/
/*
 * Turn on/off the TV emulation, redraw the image.
 */
void MCChildFrame::SetTVMode(bool b)
{
    m_bTVMode = b;
    m_pCanvas->SetEmulateTV(m_bTVMode);
}


/*****************************************************************************/
/*
 * Set the zoom scale, redraw the image.
 */
void MCChildFrame::SetScale(int nScale)
{
    m_nScale = nScale;
    m_pCanvas->SetScale(m_nScale);
}


/*****************************************************************************/
/*
 * Called when this window is activated.
 */
void MCChildFrame::OnActivate(wxActivateEvent& event)
{
	if (event.GetActive())
	{
		wxGetApp().SetActiveWindow(this);
	}

	event.Skip();
}
