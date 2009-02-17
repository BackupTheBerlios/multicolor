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
#include <wx/splitter.h>
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
    wxSplitterWindow* pSplitter = new wxSplitterWindow(this, -1);

    m_pCanvas1 = new MCCanvas(this, pSplitter, wxPoint(0, 0), wxSize(100, 100));
    m_pCanvas2 = new MCCanvas(this, pSplitter, wxPoint(0, 0), wxSize(100, 100));

    m_pCanvas1->SetVirtualSize(MC_X * 2 * m_nScale, MC_Y * m_nScale);
    m_pCanvas1->SetScrollRate(10 * m_nScale, 10 * m_nScale);

    // Canvas 2 is always scaled 1:1
    m_pCanvas2->SetVirtualSize(MC_X * 2, MC_Y);
    m_pCanvas2->SetScrollRate(10, 10);

    pSplitter->SetMinimumPaneSize(MC_X * 2);
    pSplitter->SplitVertically(m_pCanvas1, m_pCanvas2, 100);
    pSplitter->SetSashGravity(1.0);

    m_pCanvas1->CreateCache(m_nScale, m_bTVMode);
    m_pCanvas2->CreateCache(m_nScale, m_bTVMode);
}

MCChildFrame::~MCChildFrame()
{
    // TODO Auto-generated destructor stub
}

/*****************************************************************************/
/*
 * Draw the given mouse positions in both Canvas'.
 * The mouse has been moved to these bitmap coordinates.
 */
void MCChildFrame::SetMousePos(int x, int y)
{
    // Refresh prev mouse pos
    m_pCanvas1->InvalidateMouseRect();
    m_pCanvas2->InvalidateMouseRect();

    // set new pos
    m_pointMousePos.x = x;
    m_pointMousePos.y = y;

    // Refresh new mouse pos
    m_pCanvas1->InvalidateMouseRect();
    m_pCanvas2->InvalidateMouseRect();
}


/*****************************************************************************/
/*
 * Turn on/off the TV emulation, redraw the image.
 */
void MCChildFrame::SetTVMode(bool b)
{
    m_bTVMode = b;
    m_pCanvas1->DestroyCache();
    m_pCanvas1->CreateCache(m_nScale, m_bTVMode);
}


/*****************************************************************************/
/*
 * Set the zoom scale, redraw the image.
 */
void MCChildFrame::SetScale(int nScale)
{
    m_nScale = nScale;
    m_pCanvas1->DestroyCache();
    m_pCanvas1->CreateCache(m_nScale, m_bTVMode);
}


/*****************************************************************************/
// MCChildFrame drawing
void MCChildFrame::OnDraw(wxDC* pDC)
{
}

