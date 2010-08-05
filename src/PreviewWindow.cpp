/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) 2003-2010 Thomas Giesel
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
#include <wx/dcclient.h>
#include <wx/image.h>
#include <wx/utils.h>

#include "MCApp.h"
#include "PreviewWindow.h"
#include "MCMainFrame.h"
#include "MCDoc.h"

/* define this to get some extra debug effects */
//#define MC_DEBUG_REDRAW


/*****************************************************************************/
PreviewWindow::PreviewWindow(wxWindow* pParent, int nStyle) :
    wxWindow(pParent, wxID_ANY, wxDefaultPosition,
             wxSize(320, 200), nStyle | wxBG_STYLE_CUSTOM),
    m_bEmulateTV(true),
    m_pointLastMousePos(-1, -1),
    m_pointNextMousePos(-1, -1),
    m_timerRefresh(this, PREVIEWWINDOW_REFRESH_TIMER_ID),
    m_bColorPickerActive(false)
{
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(PreviewWindow::OnButtonDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(PreviewWindow::OnButtonDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(PreviewWindow::OnButtonUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(PreviewWindow::OnButtonUp));
    Connect(wxEVT_MOTION, wxMouseEventHandler(PreviewWindow::OnMouseMove));

    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(PreviewWindow::OnEraseBackground));

    Connect(wxEVT_TIMER, wxTimerEventHandler(PreviewWindow::OnTimer));
    Connect(wxEVT_PAINT, wxPaintEventHandler(PreviewWindow::OnPaint));
}


/*****************************************************************************/
/**
 * This is called when the document contents has changed, the parameters
 * report the area to be updated. Coordinates are in bitmap space.
 * x1/y1 is the upper left corner, x2/y2 is the bottom right corner.
 * It may by possible that x1 == x2 or y1 == y2 and it may be larger than
 * the actual image.
 *
 * We must force a redraw immediately, otherwise it may take a while
 * until we get to the event loop again
 *
 */
void PreviewWindow::RedrawDoc(int x1, int y1, int x2, int y2)
{
    wxRect      rect;
    BitmapBase* pB;

    if (m_pDoc)
    {
        // Calculate the rectangle to be redrawn in screen coordinates
        ToCanvasCoord(&rect.x, &rect.y, x1, y1);
        pB = m_pDoc->GetBitmap();

        // x2 = x1 is a rect with width = 1, that's why + 1
        // 2 extra pixels to b e refreshed in x direction because of TV emulation
        rect.SetWidth ((x2 - x1 + 3) * pB->GetPixelXFactor());
        rect.SetHeight((y2 - y1 + 1) * pB->GetPixelYFactor());

        RefreshRect(rect, false);
    }
    else
        Refresh(false);

    Update();
}

/*****************************************************************************/
/**
 * This is called when the mouse has been moved in one of the views.
 * Coordinates are in bitmap space.
 */
void PreviewWindow::OnDocMouseMoved(int x, int y)
{
    m_pointNextMousePos.x = x;
    m_pointNextMousePos.y = y;

    if (!m_timerRefresh.IsRunning())
    {
        m_timerRefresh.Start(50, wxTIMER_ONE_SHOT);
    }
}


/*****************************************************************************/
void PreviewWindow::OnPaint(wxPaintEvent& event)
{
    int x1, y1, x2, y2, ww, hh;
    BitmapBase* pB;

    wxPaintDC dc(this);

    if (m_pDoc)
    {
        wxRegionIterator upd(GetUpdateRegion()); // get the update rect list

        // iterate through all rectangles to be refreshed
        while (upd)
        {
            // calculate bitmap coordinates from window coordinates
            ToBitmapCoord(&x1, &y1, upd.GetX(), upd.GetY());
            ToBitmapCoord(&ww, &hh, upd.GetW(), upd.GetH());
            x2 = x1 + ww;
            y2 = y1 + hh;

            m_pDoc->GetBitmap()->SortAndClip(&x1, &y1, &x2, &y2);

            DrawScaleSmall(&dc, 1, m_bEmulateTV, x1, y1, x2, y2);
            upd++;
        }
    }
    else
    {
        // no document, draw a black box
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(*wxBLACK_BRUSH);
        dc.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    }

    DrawMousePos(&dc, m_pointLastMousePos.x, m_pointLastMousePos.y, 1);

#ifdef MC_DEBUG_REDRAW
    wxCoord x, y, w, h;
    GetUpdateRegion().GetBox(x, y, w, h);
    dc.SetPen(*wxRED_PEN);
    // dc.SetBrush(*wxTRANSPARENT_BRUSH);
    // dc.DrawRectangle(x, y, w, h);
    dc.DrawLine(x, y, x + w, y + h);
    dc.DrawLine(x + w, y, x, y + h);
#endif
}


/******************************************************************************
 * Convert window point to bitmap coordinates.
 * The coordinates are clipped to fit into the bitmap.
 *
 * Must only be called if there is a Document assigned.
 */
void PreviewWindow::ToBitmapCoord(int* px, int* py, int x, int y)
{
    BitmapBase* pB;

    if (!m_pDoc)
    {
        *px = *py = 0;
        return;
    }

    pB = m_pDoc->GetBitmap();

    *px = x / pB->GetPixelXFactor();
    *py = y / pB->GetPixelYFactor();

    if (*px < 0) *px = 0;
    if (*py < 0) *py = 0;
    if (*px > pB->GetWidth() - 1)  *px = pB->GetWidth() - 1;
    if (*py > pB->GetHeight() - 1) *py = pB->GetHeight() - 1;
}


/******************************************************************************
 * Convert bitmap point to canvas coordinates.
 */
void PreviewWindow::ToCanvasCoord(int* px, int* py, int x, int y)
{
    BitmapBase* pB;
    pB = m_pDoc->GetBitmap();

    *px = x * pB->GetPixelXFactor();
    *py = y * pB->GetPixelYFactor();
}


/*****************************************************************************/
/**
 * Enable/Disable TV emulation and delete the cache.
 */
void PreviewWindow::SetEmulateTV(bool bTV)
{
    m_bEmulateTV = bTV;
    Refresh(false);
}


/*****************************************************************************/
/**
 * Update display with our new cursor position.
 *
 * x and y are window coordinates if bCanvasCoordinates is set. Otherwise
 * they are bitmap coordinates.
 */
void PreviewWindow::UpdateCursorPosition(int x, int y, bool bCanvasCoordinates)
{
    BitmapBase* pB;

    if (m_pDoc)
    {
        if (bCanvasCoordinates)
            ToBitmapCoord(&x, &y, x, y);

        pB = m_pDoc->GetBitmap();
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > pB->GetWidth() - 1) x = pB->GetWidth() - 1;
        if (y > pB->GetHeight() - 1) y = pB->GetHeight() - 1;

        m_pDoc->SetMousePos(x, y);
    }
}


/*****************************************************************************/
void PreviewWindow::OnButtonDown(wxMouseEvent& event)
{
#if 0
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);
        Start(x, y, event.GetButton() == wxMOUSE_BTN_RIGHT);
    }
#endif
}


/*****************************************************************************/
void PreviewWindow::OnMouseMove(wxMouseEvent& event)
{
    UpdateCursorPosition(event.GetX(), event.GetY(), true);
}


/*****************************************************************************/
void PreviewWindow::OnButtonUp(wxMouseEvent& event)
{
#if 0
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);
        EndTool(x, y);
    }
#endif
}


/*****************************************************************************/
/*
 * Catch the erase background event to avoid flickering.
 */
void PreviewWindow::OnEraseBackground(wxEraseEvent& event)
{
    // do nothing
}


/*****************************************************************************/
/*
 */
void PreviewWindow::OnTimer(wxTimerEvent& event)
{
    if (event.GetId() == PREVIEWWINDOW_REFRESH_TIMER_ID)
    {
        wxClientDC dc(this);

        DrawMousePos(&dc, m_pointLastMousePos.x, m_pointLastMousePos.y, 1);
        DrawMousePos(&dc, m_pointNextMousePos.x, m_pointNextMousePos.y, 1);
        m_pointLastMousePos = m_pointNextMousePos;
    }
}
