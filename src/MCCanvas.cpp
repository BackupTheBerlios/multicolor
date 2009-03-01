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

#include <wx/docview.h>
#include <wx/dc.h>
#include <wx/image.h>
#include <wx/msgdlg.h>

#include "MCApp.h"
#include "MCCanvas.h"
#include "MCDoc.h"
#include "PalettePanel.h"
#include "MCDrawingModePanel.h"
#include "MCToolBase.h"
#include "MCMainFrame.h"
#include "MCDoc.h"

#define FIXP_SHIFT 16

/* Ein EWMA-Filter.
 * CONST ist der Exponent fuer die Filterkonstante 1 - 1/(2^CONST)
 */
#define PAINT_FILTER(F,X,CONST) ( (F)+=(X)-(F) - (((X)-(F))>>(CONST)) )

/* define this to get some extra debug effects */
//#define MC_DEBUG_REDRAW

/*****************************************************************************/
MCCanvas::MCCanvas(wxWindow* pParent, int nStyle) :
    wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, nStyle),
    m_pDoc(NULL),
    m_pActiveTool(NULL),
    m_bEmulateTV(true),
    m_nScale(1),
    m_pointLastMousePos(-1, -1)
{
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_MOTION, wxMouseEventHandler(MCCanvas::OnMouseMove));

    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MCCanvas::OnEraseBackground));

    Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(MCCanvas::OnMouseWheel));

    // set min size incl. borders so the preview won't get scroll bars
    wxSize size(GetWindowBorderSize());
    size.IncBy(2 * MC_X * m_nScale, MC_Y * m_nScale);
    SetMinSize(size);
}

/*****************************************************************************/
MCCanvas::~MCCanvas()
{
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);
}

/*****************************************************************************/
/*
 * This is called when the document contents has changed, the parameters
 * report the area to be updated. Coordinates are in bitmap space.
 *
 * We must force a redraw immediately, otherwise it may take a while
 * until we get to the event loop again
 *
 */
void MCCanvas::OnDocChanged(int x1, int y1, int x2, int y2)
{
    unsigned tmp;
    wxRect   rect;

    FixCoordinates(&x1, &y1, &x2, &y2);

    // Calculate the rectangle to be redrawn in screen coordinates
    ToCanvasCoord(&rect.x, &rect.y, x1, y1);

    // x2 = x1 is a rect with width = 1, that's why + 1
    // 2 extra pixels to b e refreshed in x direction because of TV emulation
    rect.SetWidth((x2 - x1 + 3) * 2 * m_nScale);
    rect.SetHeight((y2 - y1 + 1) * m_nScale);

    RefreshRect(rect, false);
    Update();
}

/*****************************************************************************/
/*
 * This is called when the mouse has been moved in one of the views.
 * Coordinates are in bitmap space.
 */
void MCCanvas::OnDocMouseMoved(int x, int y)
{
    InvalidateMouseRect();
    m_pointLastMousePos.x = x;
    m_pointLastMousePos.y = y;
    InvalidateMouseRect();
}

/*****************************************************************************/
/*
 * This is called when a document is destroyed which is rendered by me
 */
void MCCanvas::OnDocDestroy(MCDoc* pDoc)
{
    if (m_pDoc != pDoc)
    {
        m_pDoc = NULL;
        Refresh(false);
    }
}

/*****************************************************************************/
/*
 * Set the Document this view refers to. If it is NULL, this canvas just
 * shows a black screen from now.
 */
void MCCanvas::SetDoc(MCDoc* pDoc)
{
    // remove me from the previous document
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);

    m_pDoc = pDoc;

    // add me to the new document
    if (m_pDoc)
        m_pDoc->AddRenderer(this);

    // make sure the image will be reallocated next time it must be drawn
    m_image.Destroy();
    Refresh(false);
}

/*****************************************************************************/
/*
 * Enable/Disable TV emulation and delete the cache.
 */
void MCCanvas::SetEmulateTV(bool bTV)
{
    m_bEmulateTV = bTV;

    // make sure the image will be reallocated next time it must be drawn
    m_image.Destroy();
    Refresh(false);
}

/*****************************************************************************/
/*
 * Set zoom factor and delete the cache.
 */
void MCCanvas::SetScale(int nScale)
{
    m_nScale = nScale;

    // make sure the image will be reallocated next time it must be drawn
    m_image.Destroy();
    UpdateVirtualSize();
    Refresh(false);
}

/*****************************************************************************/
void MCCanvas::OnDraw(wxDC& rDC)
{
    int x1, y1, x2, y2;

    if (m_pDoc)
    {
        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        // iterate through all rectangles to be refreshed
        while (upd)
        {
            ToBitmapCoord(&x1, &y1, upd.GetX(), upd.GetY());
            x2 = x1 + upd.GetW() / (2 * m_nScale) + 3;
            y2 = y1 + upd.GetH() / m_nScale + 3;

            FixCoordinates(&x1, &y1, &x2, &y2);

            if (m_nScale <= 2)
                DrawScaleSmall(&rDC, x1, y1, x2, y2);
            else
                DrawScaleBig(&rDC, x1, y1, x2, y2);
            upd++;
        }

        // erase the area around the bitmap
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxGREY_BRUSH);
        rDC.DrawRectangle(0, MC_Y * m_nScale, GetSize().GetWidth(),
                GetSize().GetHeight());
        rDC.DrawRectangle(2 * MC_X * m_nScale, 0, GetSize().GetWidth(),
                GetSize().GetHeight());
    }
    else
    {
        // no document, draw a black box
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxBLACK_BRUSH);
        rDC.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    }

#ifdef MC_DEBUG_REDRAW
    wxCoord x, y, w, h;
    GetUpdateRegion().GetBox(x, y, w, h);
    rDC.SetPen(*wxRED_PEN);
    rDC.SetBrush(*wxTRANSPARENT_BRUSH);
    rDC.DrawRectangle(x, y, w, h);
#endif

    DrawMousePos(&rDC);
}


/******************************************************************************
 *
 * Paint the scaled bitmap into the cache image at scale 1:1 and 2:1.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < MC_X, 0 <= y <= MC_Y
 */
void MCCanvas::DrawScaleSmall(wxDC* pDC,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const MCBitmap* pMCB = &m_pDoc->m_bitmap;
    MC_RGB   col;
    int      fixr, fixg, fixb, tmpr, tmpg, tmpb;
    const int aFilters[] = {0, 2, 1};
    int      filter;
    int      x, y;
    unsigned char* pPixels;
    unsigned char* p;
    int            nPitch;

    filter = aFilters[m_nScale];

    if (!m_image.IsOk())
    {
        m_image.Create(2 * MC_X * m_nScale, MC_Y * m_nScale, false);
        // in this case we have to render the whole image
        x1 = 0;
        y1 = 0;
        x2 = MC_X - 1;
        y2 = MC_Y - 1;
    }

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;

    // We draw all full lines of the area so the blur has the right effect
    for (y = y1; y <= y2 * m_nScale; ++y)
    {
        fixr = fixg = fixb = 64 << FIXP_SHIFT;
        p = pPixels + y * nPitch;

        for (x = 0; x < MC_X * 2 * m_nScale; ++x)
        {
            if (x % (2 * m_nScale) == 0)
            {
                col = pMCB->GetColor(x / (2 * m_nScale), y / m_nScale)->GetRGB();
                tmpr = (col & 0xff) << FIXP_SHIFT;
                tmpg = ((col >> 8) & 0xff) << FIXP_SHIFT;
                tmpb = ((col >> 16) & 0xff) << FIXP_SHIFT;
            }
            if (m_bEmulateTV)
            {
                PAINT_FILTER(fixr, tmpr, filter);
                PAINT_FILTER(fixg, tmpg, filter);
                PAINT_FILTER(fixb, tmpb, filter);
                *p++ = fixb >> FIXP_SHIFT;
                *p++ = fixg >> FIXP_SHIFT;
                *p++ = fixr >> FIXP_SHIFT;
            }
            else
            {
                *p++ = tmpb >> FIXP_SHIFT;
                *p++ = tmpg >> FIXP_SHIFT;
                *p++ = tmpr >> FIXP_SHIFT;
            }
        } /* x */
    } /* y */

    pDC->DrawBitmap(m_image, 0, 0, false);
}

/******************************************************************************
 *
 * Draw the scaled bitmap at scale 4:1 and higher.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < MC_X, 0 <= y <= MC_Y
 */
void MCCanvas::DrawScaleBig(wxDC* pDC,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const MCBitmap* pMCB = &m_pDoc->m_bitmap;
    wxBrush        brush(*wxBLACK);
    wxPen          pen(*wxBLACK);
    MC_RGB         rgb;
    wxRect         rect;
    int            x, y, i;

    pen.SetColour(MC_GRID_COL_R, MC_GRID_COL_G, MC_GRID_COL_B);

    // Draw blocks for pixels
    pDC->SetPen(*wxTRANSPARENT_PEN);
    rect.height = m_nScale - 1;
    rect.width  = 2 * m_nScale - 1;
    for (y = y1; y <= y2; ++y)
    {
        rect.y = m_nScale * y + 1;
        for (x = x1; x <= x2; ++x)
        {
            rect.x = 2 * m_nScale * x + 1;

            rgb = pMCB->GetColor(x, y)->GetRGB();
            brush.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));

            pDC->SetBrush(brush);
            pDC->DrawRectangle(rect);
        }
    }

    // Draw the fine grid
    pDC->SetPen(pen);
    for (x = x1; x <= x2; ++x)
    {
        i = 2 * m_nScale * x;
        pDC->DrawLine(i, m_nScale * y1, i, m_nScale * (y2 + 1));
    }
    for (y = y1; y <= y2; ++y)
    {
        i = m_nScale * y;
        pDC->DrawLine(2 * m_nScale * x1, i, 2 * m_nScale * (x2 + 1), i);
    }

    // Draw the 4x8 grid, we extend the area to the next 4x8 border at the
    // upper left corner
    x1 &= ~3;
    x1 *= 2 * m_nScale;
    y1 &= ~7;
    y1 *= m_nScale;

    x2 *= 2 * m_nScale;
    y2 *= m_nScale;
    for (y = y1; y <= y2; y += 8 * m_nScale)
    {
        for (x = x1; x < x2; x += 8 * m_nScale)
        {
            rgb = pMCB->GetColor(
                x / (2 * m_nScale), y / m_nScale)->GetContrastRGB();
            pen.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
            pDC->SetPen(pen);

            pDC->DrawLine(x - m_nScale / 2, y, x + m_nScale / 2, y);
            pDC->DrawLine(x, y - m_nScale / 2, x, y + m_nScale / 2);
        }
    }
}


/******************************************************************************
 * Convert window point to bitmap coordinates. Scroll position and zoom are
 * taken into account. The coordinates are clipped to fit into the bitmap.
 */
void MCCanvas::ToBitmapCoord(int* px, int* py, int x, int y)
{
    CalcUnscrolledPosition(x, y, px, py);

    *px /= (2 * m_nScale);
    *py /= m_nScale;

    if (*px < 0) *px = 0;
    if (*py < 0) *py = 0;
    if (*px > MC_X - 1) *px = MC_X - 1;
    if (*py > MC_Y - 1) *py = MC_Y - 1;
}


/******************************************************************************
 * Convert bitmap point to canvas coordinates.
 */
void MCCanvas::ToCanvasCoord(int* px, int* py, int x, int y)
{
    *px = x * (2 * m_nScale);
    *py = y * m_nScale;
    CalcScrolledPosition(*px, *py, px, py);
}


/******************************************************************************/
/*
 * Invalidate the rectangle around the current mouse position.
 */
void MCCanvas::InvalidateMouseRect()
{
    int    x, y;
    wxRect rect;

    // Calculate the rectangle around the cursor
    x = m_pointLastMousePos.x;
    y = m_pointLastMousePos.y;
    ToCanvasCoord(&x, &y, x, y);

    rect.SetLeft(x - 8 * m_nScale);
    rect.SetTop(y - 8 * m_nScale);
    rect.SetWidth(16 * m_nScale);
    rect.SetHeight(16 * m_nScale);
    RefreshRect(rect, false);
}

/******************************************************************************/
/*
 * Draw the mouse position or remove the drawing.
 */
void MCCanvas::DrawMousePos(wxDC* pDC)
{
    int x, y;

    x = m_pointLastMousePos.x * m_nScale * 2;
    y = m_pointLastMousePos.y * m_nScale;

    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    pDC->SetPen(*wxWHITE_PEN);

    if (m_nScale >= 4)
    {
        pDC->DrawRectangle(x, y, 2 * m_nScale + 1, m_nScale + 1);
    }

    pDC->DrawLine(x - 3, y + m_nScale / 2, x, y + m_nScale / 2);
    pDC->DrawLine(x + 2 * m_nScale, y + m_nScale / 2, x + 2 * m_nScale + 3, y
            + m_nScale / 2);
    pDC->DrawLine(x + m_nScale, y - 3, x + m_nScale, y);
    pDC->DrawLine(x + m_nScale, y + m_nScale, x + m_nScale, y + m_nScale + 3);
}


/*****************************************************************************/
/*
 * Calculate and set the virtual size to get the scrollbars to the right size.
 */
void MCCanvas::UpdateVirtualSize()
{
    SetVirtualSize(MC_X * 2 * m_nScale, MC_Y * m_nScale);
    SetScrollRate(10 * m_nScale, 10 * m_nScale);
}


/*****************************************************************************/
void MCCanvas::OnButtonDown(wxMouseEvent& event)
{
    MCDrawingMode mode;
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());

        // this can happen when we didn't get ButtonUp event
        if (m_pActiveTool)
        {
            m_pActiveTool->End(x, y);
            m_pActiveTool = NULL;
        }

        m_pActiveTool = wxGetApp().GetActiveDrawingTool();

        if (m_pActiveTool)
        {
            m_pActiveTool->SetColors(wxGetApp().GetPalettePanel()->GetColorA(),
                    wxGetApp().GetPalettePanel()->GetColorB());
            m_pActiveTool->SetDoc(m_pDoc);

            mode
                    = wxGetApp().GetMainFrame()->GetToolPanel()-> GetDrawingModePanel()->GetDrawingMode();
            m_pActiveTool->SetDrawingMode(mode);

            m_pActiveTool->Start(x, y, event.GetButton() == wxMOUSE_BTN_RIGHT);
        }
    }
}


/******************************************************************************
 * Bei jeder Bewegung der Maus werden die Pixel-Koordinaten in der Statuszeile
 * angezeigt. Ausserdem wird der aktuelle Block in der linken Toolbar angezeigt.
 */
void MCCanvas::OnMouseMove(wxMouseEvent& event)
{
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());
        m_pDoc->SetMousePos(x, y);

        if (m_pActiveTool)
        {
            m_pActiveTool->Move(x, y);
        }
    }
}


/*****************************************************************************/
void MCCanvas::OnButtonUp(wxMouseEvent& event)
{
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());

        if (m_pActiveTool)
        {
            m_pActiveTool->End(x, y);
            m_pActiveTool = NULL;
        }
    }
}


/*****************************************************************************/
/*
 * Catch the erase background event to avoid flickering.
 */
void MCCanvas::OnEraseBackground(wxEraseEvent& event)
{
    // do nothing
}


/*****************************************************************************/
/*
 */
void MCCanvas::OnMouseWheel(wxMouseEvent& event)
{
#ifdef MC_NOT_YET
    int nScale = m_nScale;

    if ((event.GetWheelRotation() > 0) && (nScale > 1))
    {
        nScale /= 2;
    }

    if ((event.GetWheelRotation() < 0) && (nScale < MC_MAX_ZOOM))
    {
        nScale *= 2;
    }

    if (m_nScale != nScale)
    {
        SetScale(nScale);
    }
#endif
}


/*****************************************************************************/
/*
 * Sort and clip these coordinates so that x1/y1 <= x2/y2 and all of them are
 * clipped to the coordinate range of the document.
 */
void MCCanvas::FixCoordinates(int* px1, int* py1, int* px2, int* py2)
{
    int tmp;

    if (*px1 < 0)
        *px1 = 0;
    else if (*px1 >= MC_X)
        *px1 = MC_X - 1;

    if (*py1 < 0)
        *py1 = 0;
    else if (*py1 >= MC_Y)
        *py1 = MC_Y - 1;

    if (*px2 < 0)
        *px2 = 0;
    else if (*px2 >= MC_X)
        *px2 = MC_X - 1;

    if (*py2 < 0)
        *py2 = 0;
    else if (*py2 >= MC_Y)
        *py2 = MC_Y - 1;


    if (*px1 > *px2)
    {
        tmp  = *px1;
        *px1 = *px2;
        *px2 = tmp; // swap
    }

    if (*py1 > *py2)
    {
        tmp  = *py1;
        *py1 = *py2;
        *py2 = tmp; // swap
    }
}
