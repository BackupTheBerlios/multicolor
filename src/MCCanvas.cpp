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
#include <wx/dc.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>

#include "MCApp.h"
#include "MCCanvas.h"
#include "MCDoc.h"
#include "PalettePanel.h"
#include "MCDrawingModePanel.h"
#include "ToolBase.h"
#include "MCMainFrame.h"
#include "MCDoc.h"

#define FIXP_SHIFT 16

/* Ein EWMA-Filter.
 * CONST ist der Exponent fuer die Filterkonstante 1 - 1/(2^CONST)
 */
#define PAINT_FILTER(F,X,CONST) ( (F)+=(X)-(F) - (((X)-(F))>>(CONST)) )

/* define this to get some extra debug effects */
#define MC_DEBUG_REDRAW

std::list<MCCanvas*> MCCanvas::m_listCanvasInstances;


/*****************************************************************************/
MCCanvas::MCCanvas(wxWindow* pParent, int nStyle, bool bPreview) :
    wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition, wxSize(320, 200), nStyle),
    m_pDoc(NULL),
    m_pActiveTool(NULL),
    m_bPreviewWindow(bPreview),
    m_bEmulateTV(true),
    m_nScale(1),
    m_pointLastMousePos(-1, -1),
    m_timerScrolling(this, MCCANVAS_SCROLL_TIMER_ID),
    m_timerRefresh(this, MCCANVAS_REFRESH_TIMER_ID),
    m_bDragScrollActive(false),
    m_pointDragScrollStart(0, 0),
    m_xDragScrollStart(0),
    m_yDragScrollStart(0),
    m_bColorPickerActive(false),
    m_cursorCloneBrush(MCApp::GetImage(wxT("cursors"), wxT("clonebrush.png"))),
    m_cursorColorPicker(MCApp::GetImage(wxT("cursors"), wxT("colorpicker.png"))),
    m_cursorDots(MCApp::GetImage(wxT("cursors"), wxT("dots.png"))),
    m_cursorFloodFill(MCApp::GetImage(wxT("cursors"), wxT("floodfill.png"))),
    m_cursorFreehand(MCApp::GetImage(wxT("cursors"), wxT("freehand.png"))),
    m_cursorLines(MCApp::GetImage(wxT("cursors"), wxT("lines.png")))
{
    m_listCanvasInstances.push_back(this);

    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(MCCanvas::OnMButtonDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(MCCanvas::OnMButtonUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_MOTION, wxMouseEventHandler(MCCanvas::OnMouseMove));

    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MCCanvas::OnEraseBackground));

    if (!m_bPreviewWindow)
    {
        Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(MCCanvas::OnMouseWheel));
    }

    Connect(wxEVT_TIMER, wxTimerEventHandler(MCCanvas::OnTimer));

    Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MCCanvas::OnKeyDown));
    Connect(wxEVT_KEY_UP, wxKeyEventHandler(MCCanvas::OnKeyUp));

    UpdateCursorType();
}

/*****************************************************************************/
MCCanvas::~MCCanvas()
{
    m_listCanvasInstances.remove(this);

    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);
}

/*****************************************************************************/
/**
 * This is called when the document contents has changed, the parameters
 * report the area to be updated. Coordinates are in bitmap space.
 * x1/y1 is the upper left corner, x2/y2 is the bottom right corner.
 * It may by possible that x1 == x2 or y1 == y2. All coordinates are
 * clipped to the image size already.
 *
 * We must force a redraw immediately, otherwise it may take a while
 * until we get to the event loop again
 *
 */
void MCCanvas::OnDocChanged(int x1, int y1, int x2, int y2)
{
    wxRect      rect;
    BitmapBase* pB;

    // Calculate the rectangle to be redrawn in screen coordinates
    ToCanvasCoord(&rect.x, &rect.y, x1, y1);
    pB = m_pDoc->GetBitmap();

    // x2 = x1 is a rect with width = 1, that's why + 1
    // 2 extra pixels to b e refreshed in x direction because of TV emulation
    rect.SetWidth ((x2 - x1 + 3) * pB->GetPixelXFactor() * m_nScale);
    rect.SetHeight((y2 - y1 + 1) * pB->GetPixelYFactor() * m_nScale);

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
    if (m_timerRefresh.IsRunning())
    {
    }
    else
    {
        m_timerRefresh.Start(50, wxTIMER_ONE_SHOT);

        InvalidateMouseRect();
        m_pointLastMousePos.x = x;
        m_pointLastMousePos.y = y;
        InvalidateMouseRect();
    }
}

/*****************************************************************************/
/**
 * This is called when a document is destroyed which is rendered by me
 */
void MCCanvas::OnDocDestroy(DocBase* pDoc)
{
    if (m_pDoc == pDoc)
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
void MCCanvas::SetDoc(DocBase* pDoc)
{
    BitmapBase* pB;

    // remove me from the previous document
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);

    m_pDoc = pDoc;

    // add me to the new document
    if (m_pDoc)
        m_pDoc->AddRenderer(this);

    // set min size incl. borders so the preview won't get scroll bars
    pB = pDoc->GetBitmap();
    wxSize size(GetWindowBorderSize());
    size.IncBy(pB->GetPixelXFactor() * pB->GetWidth(),
               pB->GetPixelYFactor() * pB->GetHeight());
    SetMinSize(size);

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
void MCCanvas::SetScale(unsigned nScale)
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
    int x1, y1, x2, y2, ww, hh;
    BitmapBase* pB;

    if (m_pDoc)
    {
        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        // iterate through all rectangles to be refreshed
        while (upd)
        {
            // calculate bitmap coordinates from window coordinates
            ToBitmapCoord(&x1, &y1, upd.GetX(), upd.GetY(), true);
            ToBitmapCoord(&ww, &hh, upd.GetW(), upd.GetH(), false);
            x2 = x1 + ww;
            y2 = y1 + hh;

            m_pDoc->GetBitmap()->SortAndClip(&x1, &y1, &x2, &y2);

            if (m_nScale <= 2)
                DrawScaleSmall(&rDC, x1, y1, x2, y2);
            else
                DrawScaleBig(&rDC, x1, y1, x2, y2);
            upd++;
        }

        // erase the area around the bitmap
        pB = m_pDoc->GetBitmap();
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxGREY_BRUSH);
        rDC.DrawRectangle(
            rDC.DeviceToLogicalX(0),
            pB->GetPixelYFactor() * pB->GetHeight() * m_nScale,
            GetSize().GetWidth(), GetSize().GetHeight());
        rDC.DrawRectangle(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nScale, rDC.DeviceToLogicalY(0),
            GetSize().GetWidth(), GetSize().GetHeight());
    }
    else
    {
        // no document, draw a black box
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxBLACK_BRUSH);
        rDC.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    }

    DrawMousePos(&rDC);

#ifdef MC_DEBUG_REDRAW
    wxCoord x, y, w, h;
    GetUpdateRegion().GetBox(x, y, w, h);
    rDC.SetPen(*wxRED_PEN);
    // rDC.SetBrush(*wxTRANSPARENT_BRUSH);
    // rDC.DrawRectangle(x, y, w, h);
    rDC.DrawLine(x, y, x + w, y + h);
    rDC.DrawLine(x + w, y, x, y + h);
#endif
}


/******************************************************************************
 *
 * Paint the scaled bitmap into the cache image at scale 1:1 and 2:1.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < w, 0 <= y <= h
 *
 * x1, y1, x2, y2 are in bitmap space
 */
void MCCanvas::DrawScaleSmall(wxDC* pDC,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const BitmapBase* pB = m_pDoc->GetBitmap();
    MC_RGB   col;
    int      fixr, fixg, fixb, tmpr, tmpg, tmpb;
    const int aFilters[] = {0, 2, 1};
    int      filter;
    unsigned        x, y, w, xFactor;
    unsigned char*  pPixels;
    unsigned char*  p;
    unsigned        nPitch;

    filter = aFilters[m_nScale];

    if (!m_image.IsOk())
    {
        m_image.Create(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nScale,
            pB->GetPixelYFactor() * pB->GetHeight() * m_nScale, false);
        // in this case we have to render the whole image
        x1 = 0;
        y1 = 0;
        x2 = pB->GetWidth() - 1;
        y2 = pB->GetHeight() - 1;
    }

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;
    xFactor = pB->GetPixelXFactor() * m_nScale;
    w       = xFactor * pB->GetWidth();

    // We draw all full lines of the area so the blur has the right effect
    for (y =  y1 * pB->GetPixelYFactor();
         y <= y2 * pB->GetPixelYFactor() * m_nScale;
         ++y)
    {
        fixr = fixg = fixb = 64 << FIXP_SHIFT;
        p = pPixels + y * nPitch;

        for (x = 0; x < w; ++x)
        {
            if (x % xFactor == 0)
            {
                col = pB->GetColor(x / xFactor, y / m_nScale)->GetRGB();
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
 * x1 <= x2, y1 <= y2, 0 <= x < w, 0 <= y <= h
 *
 * x1, y1, x2, y2 are in bitmap space
 */
void MCCanvas::DrawScaleBig(wxDC* pDC,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const BitmapBase* pB = m_pDoc->GetBitmap();
    wxBrush        brush(*wxBLACK);
    wxPen          pen(*wxBLACK);
    MC_RGB         rgb;
    wxRect         rect;
    unsigned       x, y, i, xFactor;

    xFactor = pB->GetPixelXFactor() * m_nScale;

    pen.SetColour(MC_GRID_COL_R, MC_GRID_COL_G, MC_GRID_COL_B);

    // Draw blocks for pixels
    pDC->SetPen(*wxTRANSPARENT_PEN);
    rect.height = m_nScale - 1;
    rect.width  = xFactor - 1;
    for (y = y1; y <= y2; ++y)
    {
        rect.y = m_nScale * y + 1;
        for (x = x1; x <= x2; ++x)
        {
            rect.x = xFactor * x + 1;

            rgb = pB->GetColor(x, y)->GetRGB();
            brush.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));

            pDC->SetBrush(brush);
            pDC->DrawRectangle(rect);
        }
    }

    // Draw the fine grid
    pDC->SetPen(pen);
    for (x = x1; x <= x2; ++x)
    {
        i = xFactor * x;
        pDC->DrawLine(i, m_nScale * y1, i, m_nScale * (y2 + 1));
    }
    for (y = y1; y <= y2; ++y)
    {
        i = m_nScale * y;
        pDC->DrawLine(xFactor * x1, i, xFactor * (x2 + 1), i);
    }

    // Draw the cell grid, we extend the area to the next border at the
    // upper left corner
    x1 -= x1 % pB->GetCellWidth();
    x1 *= xFactor;
    y1 -= y1 % pB->GetCellHeight();
    y1 *= m_nScale;

    x2 *= xFactor;
    y2 *= m_nScale;
    for (y = y1; y <= y2; y += 8 * m_nScale)
    {
        for (x = x1; x < x2; x += 8 * m_nScale)
        {
            rgb = pB->GetColor(
                x / xFactor, y / m_nScale)->GetContrastRGB();
            pen.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
            pDC->SetPen(pen);

            pDC->DrawLine(x - m_nScale / 2, y, x + m_nScale / 2, y);
            pDC->DrawLine(x, y - m_nScale / 2, x, y + m_nScale / 2);
        }
    }
}


/******************************************************************************
 * Convert window point to bitmap coordinates. Scroll position is taken into
 * account if bScroll ist true. Zoom is always used.
 * The coordinates are clipped to fit into the bitmap.
 *
 * Must only be called if there is a Document assigned.
 */
void MCCanvas::ToBitmapCoord(int* px, int* py, int x, int y, bool bScroll)
{
    BitmapBase* pB;

    if (!m_pDoc)
    {
        *px = *py = 0;
        return;
    }

    pB = m_pDoc->GetBitmap();

    CalcUnscrolledPosition(x, y, px, py);

    *px /= pB->GetPixelXFactor() * (int)m_nScale;
    *py /= pB->GetPixelYFactor() * (int)m_nScale;

    if (*px < 0) *px = 0;
    if (*py < 0) *py = 0;
    if (*px > pB->GetWidth() - 1)  *px = pB->GetWidth() - 1;
    if (*py > pB->GetHeight() - 1) *py = pB->GetHeight() - 1;
}


/******************************************************************************
 * Convert bitmap point to canvas coordinates.
 */
void MCCanvas::ToCanvasCoord(int* px, int* py, int x, int y)
{
    BitmapBase* pB;
    pB = m_pDoc->GetBitmap();

    *px = x * pB->GetPixelXFactor() * m_nScale;
    *py = y * pB->GetPixelYFactor() * m_nScale;
    CalcScrolledPosition(*px, *py, px, py);
}


/******************************************************************************/
/*
 * Scroll the window so the given bitmap coordinate is centered in the window,
 * if possible.
 */
void MCCanvas::CenterBitmapPoint(int x, int y)
{
    int xScroll, yScroll;
    int xFactor, yFactor;
    int wClient, hClient;
    BitmapBase* pB;

    pB = m_pDoc->GetBitmap();

    GetClientSize(&wClient, &hClient);
    GetScrollPixelsPerUnit(&xFactor, &yFactor);

    x *= pB->GetPixelXFactor() * m_nScale;
    y *= pB->GetPixelYFactor() * m_nScale;

    xScroll = (x - wClient / 2) / xFactor;
    yScroll = (y - hClient / 2) / yFactor;
    Scroll(xScroll, yScroll);
}


/******************************************************************************/
/*
 * Update the cursors of all Canvases.
 */
void MCCanvas::UpdateAllCursorTypes()
{
    std::list<MCCanvas*>::iterator i;

    for (i = m_listCanvasInstances.begin();
         i != m_listCanvasInstances.end(); ++i)
    {
        (*i)->UpdateCursorType();
    }
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

    rect.SetLeft(x - m_nScale);
    rect.SetTop(y - m_nScale);
    rect.SetWidth(3 * m_nScale);
    rect.SetHeight(3 * m_nScale);
    RefreshRect(rect, false);
}


/******************************************************************************/
/*
 * Draw the mouse position or remove the drawing.
 */
void MCCanvas::DrawMousePos(wxDC* pDC)
{
    int x, y, xFactor, yFactor, c;
    BitmapBase* pB;

    if (!m_pDoc)
        return;

    pB = m_pDoc->GetBitmap();
    xFactor = pB->GetPixelXFactor() * m_nScale;
    yFactor = pB->GetPixelYFactor() * m_nScale;

    x = m_pointLastMousePos.x * xFactor;
    y = m_pointLastMousePos.y * yFactor;

    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    pDC->SetPen(*wxWHITE_PEN);

    if (m_nScale >= 4)
    {
        pDC->DrawRectangle(x, y, xFactor + 1, yFactor + 1);
    }

    c = y + yFactor / 2;
    pDC->DrawLine(x - 3,           c,
                  x,               c);
    pDC->DrawLine(x + xFactor,     c,
                  x + xFactor + 3, c);

    c = x + xFactor / 2;
    pDC->DrawLine(c, y - 3,
                  c, y);
    pDC->DrawLine(c, y + yFactor,
                  c, y + yFactor + 3);
}


/*****************************************************************************/
/*
 * Calculate and set the virtual size to get the scrollbars to the right size.
 */
void MCCanvas::UpdateVirtualSize()
{
    BitmapBase* pB;

    if (m_pDoc)
    {
        pB = m_pDoc->GetBitmap();
        SetVirtualSize(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nScale,
            pB->GetPixelYFactor() * pB->GetHeight() * m_nScale);
        SetScrollRate(
            pB->GetPixelXFactor() * m_nScale,
            pB->GetPixelYFactor() * m_nScale);
    }
}


/*****************************************************************************/
/*
 * Check if we have to scroll. If yes, do it.
 * This is called upon mouse movement and when the scroll timer elapsed.
 * If we have to scroll we will start the timer (again) to re-check later.
 *
 * xMouse and yMouse are window coordinates.
 *
 * Return true if we scrolled and false if not.
 */
bool MCCanvas::CheckScrolling(int xMouse, int yMouse)
{
    int xScroll, yScroll;
    int xScrollOld, yScrollOld;
    int xFactor, yFactor;
    int wClient, hClient;
    int wVirtual, hVirtual;

    GetViewStart(&xScroll, &yScroll);
    xScrollOld = xScroll;
    yScrollOld = yScroll;

    GetVirtualSize(&wVirtual, &hVirtual);
    GetClientSize(&wClient, &hClient);
    GetScrollPixelsPerUnit(&xFactor, &yFactor);
    if (xFactor == 0) xFactor = 1;
    if (yFactor == 0) yFactor = 1;

    xScroll = CheckScrollingOneDirection(
            xScrollOld, xMouse, wClient, (wVirtual - wClient) / xFactor);

    yScroll = CheckScrollingOneDirection(
            yScrollOld, yMouse, hClient, (hVirtual - hClient) / yFactor);

    // Are all conditions for scrolling fulfilled?
    if (xScroll != xScrollOld || yScroll != yScrollOld)
    {
        // If the timer is running do not scroll now but when it'll expire
        if (!m_timerScrolling.IsRunning())
        {
            // Scroll now and restart the timer
            Scroll(xScroll, yScroll);
            m_timerScrolling.Start(MCCANVAS_SCROLL_INTERVAL, wxTIMER_ONE_SHOT);
            return true;
        }
    }
    else
    {
        // No need to scroll anymore, just stop the timer
        m_timerScrolling.Stop();
    }
    return false;
}


/*****************************************************************************/
/*
 * This function is used by CheckScrolling for calculating the scrolling
 * in horizontal or in vertical direction.
 *
 * It checks following:
 * Scroll left if the mouse is left of the threshold AND
 * (the mouse in inside the window OR a tool is active) AND
 * there's still space for scrolling
 *
 * return the new scroll position
 */
int MCCanvas::CheckScrollingOneDirection(
        int nScroll, int nMousePos, int nAreaMax, int nScrollMax)
{
    if (nMousePos < MCCANVAS_SCROLL_THRESHOLD &&
        (nMousePos >= 0 || m_pActiveTool))
    {
        nScroll -= 8;
        if (nScroll < 0)
            nScroll = 0;
    }
    else if (nMousePos >= nAreaMax - MCCANVAS_SCROLL_THRESHOLD &&
             (nMousePos < nAreaMax || m_pActiveTool))
    {
        nScroll += 8;
        if (nScroll > nScrollMax)
            nScroll = nScrollMax;
    }
    return nScroll;
}


/*****************************************************************************/
/*
 * Move the drawing cursor with a key. If the cursor leaves the visible
 * area, scroll it to the center of the screen.
 */
void MCCanvas::MoveCursorWithKey(int nKeyCode)
{
    BitmapBase* pB;
    int  x, y;
    int  xCanvas, yCanvas;
    bool bMoved = false;

    if (!m_pDoc)
        return;

    pB = m_pDoc->GetBitmap();

    x = m_pointLastMousePos.x;
    y = m_pointLastMousePos.y;

    switch (nKeyCode)
    {
    case WXK_UP:
        if (y > 0)
        {
            --y;
            bMoved = true;
        }
        break;

    case WXK_DOWN:
        if (y < pB->GetHeight() - 1)
        {
            ++y;
            bMoved = true;
        }
        break;

    case WXK_LEFT:
        if (x > 0)
        {
            --x;
            bMoved = true;
        }
        break;

    case WXK_RIGHT:
        if (x < pB->GetWidth() - 1)
        {
            ++x;
            bMoved = true;
        }
        break;
    }

    if (bMoved)
    {
        UpdateCursorPosition(x, y, false);

        ToCanvasCoord(&xCanvas, &yCanvas, x, y);
        if (xCanvas < MCCANVAS_SCROLL_THRESHOLD ||
            xCanvas >= GetClientSize().GetWidth() - MCCANVAS_SCROLL_THRESHOLD ||
            yCanvas < MCCANVAS_SCROLL_THRESHOLD ||
            yCanvas >= GetClientSize().GetHeight() - MCCANVAS_SCROLL_THRESHOLD)
        {
            CenterBitmapPoint(x, y);
        }
    }
}


/*****************************************************************************/
/*
 * Start a tool at the given coordinate.
 */
void MCCanvas::StartTool(int x, int y, bool bSecondary)
{
    MCDrawingMode mode;

    // this can happen when we didn't get ButtonUp event
    if (m_pActiveTool)
        return;

    if (m_bColorPickerActive)
        m_pActiveTool = wxGetApp().GetDrawingTool(MC_ID_TOOL_COLOR_PICKER);
    else
        m_pActiveTool = wxGetApp().GetDrawingTool();

    if (m_pActiveTool)
    {
        m_pActiveTool->SetColors(wxGetApp().GetPalettePanel()->GetColorA(),
                wxGetApp().GetPalettePanel()->GetColorB());
        m_pActiveTool->SetDoc(m_pDoc);

        mode = wxGetApp().GetMainFrame()->
            GetToolPanel()-> GetDrawingModePanel()->GetDrawingMode();
        m_pActiveTool->SetDrawingMode(mode);

        m_pActiveTool->Start(x, y, bSecondary);
        m_pDoc->RefreshDirty();
    }
}


/*****************************************************************************/
/*
 * Update display and tool with our new cursor position.
 *
 * x and y are window coordinates if bCanvasCoordinates is set. Otherwise
 * they are bitmap coordinates.
 */
void MCCanvas::UpdateCursorPosition(int x, int y, bool bCanvasCoordinates)
{
    BitmapBase* pB;

    if (m_pDoc)
    {
        if (bCanvasCoordinates)
            ToBitmapCoord(&x, &y, x, y, true);

        pB = m_pDoc->GetBitmap();
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > pB->GetWidth() - 1) x = pB->GetWidth() - 1;
        if (y > pB->GetHeight() - 1) y = pB->GetHeight() - 1;

        m_pDoc->SetMousePos(x, y);

        if (m_pActiveTool)
        {
            m_pActiveTool->Move(x, y);
            m_pDoc->RefreshDirty();
        }
    }
}


/*****************************************************************************/
/*
 * End the tool if one is active.
 */
void MCCanvas::EndTool(int x, int y)
{
    if (m_pActiveTool)
    {
        m_pActiveTool->End(x, y);
        m_pActiveTool = NULL;
        m_pDoc->RefreshDirty();
    }
}


/*****************************************************************************/
/*
 * Set the right cursor type (bitmap) for the active tool.
 */
void MCCanvas::UpdateCursorType()
{
    ToolBase* pSelectedTool;
    int idTool;

    if (m_bColorPickerActive)
        pSelectedTool = wxGetApp().GetDrawingTool(MC_ID_TOOL_COLOR_PICKER);
    else
        pSelectedTool = wxGetApp().GetDrawingTool();

    if (pSelectedTool)
            idTool = pSelectedTool->GetToolId();

    switch (idTool)
    {
    case MC_ID_TOOL_CLONE_BRUSH:
        SetCursor(m_cursorCloneBrush);
        break;

    case MC_ID_TOOL_DOTS:
        SetCursor(m_cursorDots);
        break;

    case MC_ID_TOOL_FILL:
        SetCursor(m_cursorFloodFill);
        break;

    case MC_ID_TOOL_FREEHAND:
        SetCursor(m_cursorFreehand);
        break;

    case MC_ID_TOOL_LINES:
        SetCursor(m_cursorLines);
        break;

    case MC_ID_TOOL_COLOR_PICKER:
        SetCursor(m_cursorColorPicker);
        break;

    default:
        SetCursor(wxNullCursor);
    }
}

/*****************************************************************************/
void MCCanvas::OnButtonDown(wxMouseEvent& event)
{
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);
        StartTool(x, y, event.GetButton() == wxMOUSE_BTN_RIGHT);
    }
}


/*****************************************************************************/
void MCCanvas::OnMouseMove(wxMouseEvent& event)
{
    int xFactor, yFactor;
    int xScroll, yScroll;

    if (m_bDragScrollActive)
    {
        GetScrollPixelsPerUnit(&xFactor, &yFactor);
        if (xFactor == 0) xFactor = 1;
        if (yFactor == 0) yFactor = 1;

        xScroll = m_xDragScrollStart -
                  (event.GetX() - m_pointDragScrollStart.x) / xFactor;
        yScroll = m_yDragScrollStart -
                  (event.GetY() - m_pointDragScrollStart.y) / yFactor;

        Scroll(xScroll, yScroll);
    }
#ifndef N2C
    else if (!m_timerScrolling.IsRunning())
    {
        m_timerScrolling.Start(MCCANVAS_SCROLL_DELAY, MCCANVAS_SCROLL_TIMER_ID);
    }
#endif

    UpdateCursorPosition(event.GetX(), event.GetY(), true);
}


/*****************************************************************************/
void MCCanvas::OnButtonUp(wxMouseEvent& event)
{
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);
        EndTool(x, y);
    }
}


/*****************************************************************************/
/*
 *
 */
void MCCanvas::OnMButtonDown(wxMouseEvent& event)
{
    m_bDragScrollActive = true;
    m_pointDragScrollStart = event.GetPosition();
    GetViewStart(&m_xDragScrollStart, &m_yDragScrollStart);
}


/*****************************************************************************/
/*
 *
 */
void MCCanvas::OnMButtonUp(wxMouseEvent& event)
{
    m_bDragScrollActive = false;
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
    unsigned nScale = m_nScale;
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);

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
            CenterBitmapPoint(x, y);
        }
    }
}

/*****************************************************************************/
/*
 * Handle events for pressed keys.
 */
void MCCanvas::OnKeyDown(wxKeyEvent& event)
{
    // We also use key events in MCMainFrame, that's why we propagate them
    bool bPropagateEvent = true;

    switch (event.GetKeyCode())
    {
    case WXK_UP:
    case WXK_DOWN:
    case WXK_LEFT:
    case WXK_RIGHT:
        MoveCursorWithKey(event.GetKeyCode());
        // We eat this event so it doesn't scroll automatically
        bPropagateEvent = false;
        break;

    case 'C':
        StartTool(m_pointLastMousePos.x, m_pointLastMousePos.y, false);
        break;

    case 'V':
        StartTool(m_pointLastMousePos.x, m_pointLastMousePos.y, true);
        break;

    case WXK_SHIFT:
        m_bColorPickerActive = true;
        UpdateCursorType();
        bPropagateEvent = true;
        break;
    }

    if (bPropagateEvent)
    {
        event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
        event.Skip();
    }
}


/*****************************************************************************/
/*
 * Handle events for released keys.
 */
void MCCanvas::OnKeyUp(wxKeyEvent& event)
{
    // We also use key events in MCMainFrame, that's why we propagate them
    bool bPropagateEvent = true;

    switch (event.GetKeyCode())
    {
    case WXK_UP:
    case WXK_DOWN:
    case WXK_LEFT:
    case WXK_RIGHT:
        // We eat this event so it doesn't scroll automatically
        bPropagateEvent = false;
        break;

    case 'C':
    case 'V':
        EndTool(m_pointLastMousePos.x, m_pointLastMousePos.y);
        break;

    case WXK_SHIFT:
        m_bColorPickerActive = false;
        UpdateCursorType();
        bPropagateEvent = true;
        break;
    }

    if (bPropagateEvent)
    {
        event.ResumePropagation(wxEVENT_PROPAGATE_MAX);
        event.Skip();
    }
}


/*****************************************************************************/
/*
 */
void MCCanvas::OnTimer(wxTimerEvent& event)
{
    bool bScrolled;

    if (event.GetId() == MCCANVAS_SCROLL_TIMER_ID)
    {
        wxPoint pos(wxGetMousePosition());
        ScreenToClient(&pos.x, &pos.y);

        bScrolled = CheckScrolling(pos.x, pos.y);

        if (bScrolled)
        {
            UpdateCursorPosition(pos.x, pos.y, true);
        }
    }
}
