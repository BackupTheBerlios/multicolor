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

#include <wx/event.h>
#include <wx/dcclient.h>
#include <wx/dc.h>

#include "MCBlock.h"
#include "MCBlockPanel.h"
#include "C64Color.h"
#include "MCDoc.h"

/*****************************************************************************/
/**
 * Create a palette panel.
 */
MCBlockPanel::MCBlockPanel(wxWindow* pParent) :
        wxWindow(pParent, wxID_ANY, wxDefaultPosition, wxSize(m_nWTotal, m_nHTotal)),
        m_pDoc(NULL),
        m_timerRefresh(this, MCBLOCKPANEL_REFRESH_TIMER_ID)
{
    SetMinSize(GetSize());
    SetMaxSize(GetSize());

    Connect(wxEVT_TIMER, wxTimerEventHandler(MCBlockPanel::OnTimer));
    Connect(wxEVT_PAINT, wxPaintEventHandler(MCBlockPanel::OnPaint));
}


/*****************************************************************************/
MCBlockPanel::~MCBlockPanel()
{
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);
}

/*****************************************************************************/
/**
 * This is called when the document contents has changed, the parameters
 * report the area to be updated. Coordinates are in bitmap space.
 */
void MCBlockPanel::RedrawDoc(int x1, int y1, int x2, int y2)
{
    Refresh(false);
}


/*****************************************************************************/
/**
 * This is called when the mouse has been moved in one of the views.
 * Coordinates are in bitmap space.
 */
void MCBlockPanel::OnDocMouseMoved(int x, int y)
{
    if (!m_timerRefresh.IsRunning())
        m_timerRefresh.Start(MCBLOCKPANEL_UPDATE_INTERVAL, wxTIMER_ONE_SHOT);
}


/*****************************************************************************/
/**
 * Paint it.
 */
void MCBlockPanel::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    if (m_pDoc)
        DrawBlock(&dc, m_pDoc, m_pDoc->GetMousePos().x, m_pDoc->GetMousePos().y);
}


/*****************************************************************************/
/**
 * This is called when the timer elapses. The block will be refreshed.
 */
void MCBlockPanel::OnTimer(wxTimerEvent& event)
{
    if (event.GetId() == MCBLOCKPANEL_REFRESH_TIMER_ID)
    {
        Refresh(false);
    }
}


/*****************************************************************************/
/**
 * Show the block which contains the given bitmap coordinates.
 */
void MCBlockPanel::DrawBlock(wxDC* pDC, DocBase* pDoc, unsigned x, unsigned y)
{
    int       xx, yy, index, count;
    int       xCell, yCell, wCell, hCell;
    int       nIndexes;
    MC_RGB    rgb;
    wxRect    rect;
    wxBrush   brush(*wxBLACK);
    wxPen     pen(*wxBLACK);
    wxString  str;
    wxSize    textExtent;

    BitmapBase* pB = pDoc->GetBitmap();

    pen.SetColour(MC_GRID_COL_R, MC_GRID_COL_G, MC_GRID_COL_B);

    // cell size
    wCell = pDoc->GetBitmap()->GetCellWidth();
    hCell = pDoc->GetBitmap()->GetCellHeight();

    // make x and y cell-aligned
    xCell = x - x % wCell;
    yCell = y - y % hCell;

    // draw the block of pixels
    rect.width  = pB->GetPixelXFactor() * m_nWBox;
    rect.height = pB->GetPixelYFactor() * m_nHBox;
    rect.y = m_nWBorder;
    for (yy = 0; yy < hCell; ++yy)
    {
        rect.x = m_nWBorder;
        for (xx = 0; xx < wCell; ++xx)
        {
            if ((xx == x % wCell) && (yy == y % hCell))
                pDC->SetPen(*wxWHITE_PEN);
            else
                pDC->SetPen(pen);

            rgb = pB->GetColor(xCell + xx, yCell + yy)->GetRGB();
            brush.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
            pDC->SetBrush(brush);
            pDC->DrawRectangle(rect);
            rect.x += rect.width;
        }
        rect.y += rect.height;
    }

    // 4 little labels C0..C3
    pDC->SetTextForeground(*wxBLACK);
    rect.height = 2 * m_nHBox;
    rect.width = 4 * m_nWBox;
    // x depends from code above!
    rect.x += m_nWBorder;
    rect.y  = m_nWBorder;
    nIndexes = pB->GetNIndexes();

    for (index = 0; index < nIndexes; ++index)
    {
        str = wxString::Format(wxT("C%d"), index);
        textExtent = pDC->GetTextExtent(str);

        pDC->DrawText(str, rect.x, rect.y + (rect.height - textExtent.y) / 2);
        rect.y += 2 * m_nHBox;
    }

    // count the colors
    pDC->SetPen(*wxBLACK_PEN);
    rect.x += textExtent.x + 1;
    rect.y = m_nWBorder;
    for (index = 0; index < nIndexes; ++index)
    {
        rgb   = pB->GetColorByIndex(xCell, yCell, index)->GetRGB();
        count = pB->CountColorByIndex(xCell, yCell, index);

        brush.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
        pDC->SetBrush(brush);
        pDC->DrawRectangle(rect);

        if (MC_RGB_R(rgb) + MC_RGB_G(rgb) + MC_RGB_B(rgb) > 3 * 128)
        {
            pDC->SetTextForeground(*wxBLACK);
        }
        else
        {
            pDC->SetTextForeground(*wxWHITE);
        }

        str = wxString::Format(wxT("%dx"), count);
        textExtent = pDC->GetTextExtent(str);

        pDC->DrawText(str,
                      rect.x + (rect.width - textExtent.x) / 2,
                      rect.y + (rect.height - textExtent.y) / 2);
        rect.y += 2 * m_nHBox;
    }
}
