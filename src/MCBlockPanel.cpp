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
        m_pDoc(NULL)
{
    SetMinSize(GetSize());
    SetMaxSize(GetSize());

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
void MCBlockPanel::OnDocChanged(int x1, int y1, int x2, int y2)
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
    Refresh(false);
}


/*****************************************************************************/
/**
 * This is called when a document is destroyed which is rendered by me
 */
void MCBlockPanel::OnDocDestroy(MCDoc* pDoc)
{
    if (m_pDoc == pDoc)
    {
        m_pDoc = NULL;
        Refresh(false);
    }
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
    else
    {
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(
            m_nWBorder, m_nWBorder,
            MCBLOCK_WIDTH * m_nWBox, MCBLOCK_HEIGHT * m_nHBox);
    }
}


/*****************************************************************************/
/**
 * Set the Document this view refers to. If it is NULL, this preview just
 * shows a black block from now.
 */
void MCBlockPanel::SetDoc(MCDoc* pDoc)
{
    // remove me from the previous document
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);

    m_pDoc = pDoc;

    // add me to the new document
    if (m_pDoc)
        m_pDoc->AddRenderer(this);

    Refresh(false);
}


/*****************************************************************************/
/**
 * Show the block which contains the given bitmap coordinates.
 */
void MCBlockPanel::DrawBlock(wxDC* pDC, MCDoc* pDoc, unsigned x, unsigned y)
{
    unsigned  xx, yy;
    MC_RGB    rgb;
    wxRect    rect;
    wxBrush   brush(*wxBLACK);
    wxPen     pen(*wxBLACK);
    wxString  str;
    wxSize    textExtent;

    MCBitmap* pMCB = (MCBitmap*) pDoc->GetBitmap();
    const MCBlock* pBlock = pMCB->GetMCBlock(pDoc->GetMousePos().x, y);
    if (!pBlock)
        return;

    pen.SetColour(MC_GRID_COL_R, MC_GRID_COL_G, MC_GRID_COL_B);

    // draw the block of pixels
    rect.width = m_nWBox;
    rect.height = m_nHBox;
    rect.y = m_nWBorder;
    for (yy = 0; yy < MCBLOCK_HEIGHT; ++yy)
    {
        rect.x = m_nWBorder;
        for (xx = 0; xx < MCBLOCK_WIDTH; ++xx)
        {
            if ((xx == x % MCBLOCK_WIDTH) &&
                    (yy == y % MCBLOCK_HEIGHT))
            {
                pDC->SetPen(*wxWHITE_PEN);
            }
            else
            {
                pDC->SetPen(pen);
            }

            rgb = pBlock->GetColor(xx, yy)->GetRGB();
            brush.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
            pDC->SetBrush(brush);
            pDC->DrawRectangle(rect);
            rect.x += m_nWBox;
        }
        rect.y += m_nHBox;
    }

    // 4 little strings C0..C3
    pDC->SetTextForeground(*wxBLACK);
    rect.height = 2 * m_nHBox;
    rect.width = 2 * m_nWBox;
    rect.x = m_nWBorder + MCBLOCK_WIDTH * m_nWBox + m_nWBorder;
    rect.y = m_nWBorder;
    for (yy = 0; yy < 4; ++yy)
    {
        str = wxString::Format(wxT("C%d"), yy);
        textExtent = pDC->GetTextExtent(str);

        pDC->DrawText(str, rect.x, rect.y + (rect.height - textExtent.y) / 2);
        rect.y += 2 * m_nHBox;
    }

    // count the 4 colors
    pDC->SetPen(*wxBLACK_PEN);
    rect.x += textExtent.x + 1;
    rect.y = m_nWBorder;
    for (yy = 0; yy < 4; ++yy)
    {
        rgb = pBlock->GetMCColor(yy)->GetRGB();
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

        str = wxString::Format(wxT("%dx"), pBlock->CountMCIndex(yy));
        textExtent = pDC->GetTextExtent(str);

        pDC->DrawText(str,
                      rect.x + (rect.width - textExtent.x) / 2,
                      rect.y + (rect.height - textExtent.y) / 2);
        rect.y += 2 * m_nHBox;
    }
}
