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

/* define this to get some extra debug effects */
//#define MC_DEBUG_REDRAW


/*****************************************************************************/
MCCanvas::MCCanvas(wxWindow* pParent, int nStyle) :
    wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition,
                     wxSize(320, 200), nStyle | wxBG_STYLE_CUSTOM),
    m_bEmulateTV(true),
    m_nZoom(1),
    m_pActiveTool(NULL),
    m_pointLastMousePos(-1, -1),
    m_pointNextMousePos(-1, -1),
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
    m_cursorLines(MCApp::GetImage(wxT("cursors"), wxT("lines.png"))),
    m_cursorRect(MCApp::GetImage(wxT("cursors"),wxT("rect.png")))
{
    Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(MCCanvas::OnMButtonDown));
    Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MCCanvas::OnButtonDown));
    Connect(wxEVT_LEFT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler(MCCanvas::OnMButtonUp));
    Connect(wxEVT_RIGHT_UP, wxMouseEventHandler(MCCanvas::OnButtonUp));
    Connect(wxEVT_MOTION, wxMouseEventHandler(MCCanvas::OnMouseMove));

    Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(MCCanvas::OnEraseBackground));
    Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(MCCanvas::OnMouseWheel));

    Connect(wxEVT_TIMER, wxTimerEventHandler(MCCanvas::OnTimer));

    Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MCCanvas::OnKeyDown));
    Connect(wxEVT_KEY_UP, wxKeyEventHandler(MCCanvas::OnKeyUp));

    UpdateCursorType();
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
void MCCanvas::RedrawDoc(int x1, int y1, int x2, int y2)
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
        rect.SetWidth ((x2 - x1 + 3) * pB->GetPixelXFactor() * m_nZoom);
        rect.SetHeight((y2 - y1 + 1) * pB->GetPixelYFactor() * m_nZoom);

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
void MCCanvas::OnDocMouseMoved(int x, int y)
{
    m_pointNextMousePos.x = x;
    m_pointNextMousePos.y = y;

    if (!m_timerRefresh.IsRunning())
        m_timerRefresh.Start(20, wxTIMER_ONE_SHOT);
}


/*****************************************************************************/
/**
 * Set the document this renderer has to show from now. May be NULL if
 * there is no document attached.
 *
 * If it is NULL, this canvas just shows a black screen.
 */
void MCCanvas::SetDoc(DocBase* pDoc)
{
    BitmapBase* pB;

    if (pDoc)
    {
        // set min size incl. borders
        pB = pDoc->GetBitmap();
        wxSize size(GetWindowBorderSize());
        size.IncBy(pB->GetPixelXFactor() * pB->GetWidth(),
                   pB->GetPixelYFactor() * pB->GetHeight());
        SetMinSize(size);
    }

    DocRenderer::SetDoc(pDoc);
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

            if (m_nZoom <= 2)
                DrawScaleSmall(&rDC, m_nZoom, m_bEmulateTV, x1, y1, x2, y2);
            else
                DrawScaleBig(&rDC, m_nZoom, x1, y1, x2, y2);
            upd++;
        }

        // erase the area around the bitmap
        pB = m_pDoc->GetBitmap();
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxGREY_BRUSH);
        rDC.DrawRectangle(
            rDC.DeviceToLogicalX(0),
            pB->GetPixelYFactor() * pB->GetHeight() * m_nZoom,
            GetSize().GetWidth(), GetSize().GetHeight());
        rDC.DrawRectangle(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nZoom, rDC.DeviceToLogicalY(0),
            GetSize().GetWidth(), GetSize().GetHeight());
    }
    else
    {
        // no document, draw a black box
        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxBLACK_BRUSH);
        rDC.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
    }

    DrawMousePos(&rDC, m_pointLastMousePos.x, m_pointLastMousePos.y, m_nZoom);

#ifdef MC_DEBUG_REDRAW
    wxCoord x, y, w, h;
    GetUpdateRegion().GetBox(x, y, w, h);
    rDC.SetPen(*wxRED_PEN);
    // rDC.SetBrush(*wxTRANSPARENT_BRUSH);
    // rDC.DrawRectangle(x, y, w, h);
    CalcUnscrolledPosition(x, y, &x, &y);
    rDC.DrawLine(x, y, x + w, y + h);
    rDC.DrawLine(x + w, y, x, y + h);
#endif
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

    *px /= pB->GetPixelXFactor() * (int)m_nZoom;
    *py /= pB->GetPixelYFactor() * (int)m_nZoom;

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

    *px = x * pB->GetPixelXFactor() * m_nZoom;
    *py = y * pB->GetPixelYFactor() * m_nZoom;
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

    x *= pB->GetPixelXFactor() * m_nZoom;
    y *= pB->GetPixelYFactor() * m_nZoom;

    xScroll = (x - wClient / 2) / xFactor;
    yScroll = (y - hClient / 2) / yFactor;
    Scroll(xScroll, yScroll);
}


/*****************************************************************************/
/**
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
/**
 * Enable/Disable TV emulation and delete the cache.
 */
void MCCanvas::SetEmulateTV(bool bTV)
{
    m_bEmulateTV = bTV;
    Refresh(false);
}


/*****************************************************************************/
/**
 * Set zoom factor and delete the cache.
 */
void MCCanvas::SetZoom(unsigned nZoom)
{
    BitmapBase* pB;

    m_nZoom = nZoom;

    if (m_pDoc)
    {
        pB = m_pDoc->GetBitmap();
        SetVirtualSize(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nZoom,
            pB->GetPixelYFactor() * pB->GetHeight() * m_nZoom);
        SetScrollRate(
            pB->GetPixelXFactor() * m_nZoom,
            pB->GetPixelYFactor() * m_nZoom);
    }

    Refresh(false);
}


/*****************************************************************************/
/**
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

    case MC_ID_TOOL_RECT:
        SetCursor(m_cursorRect);
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
    unsigned nZoom = m_nZoom;
    int x, y;

    if (m_pDoc)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY(), true);

        if ((event.GetWheelRotation() > 0) && (nZoom > 1))
        {
            nZoom /= 2;
        }

        if ((event.GetWheelRotation() < 0) && (nZoom < MC_MAX_ZOOM))
        {
            nZoom *= 2;
        }

        if (m_nZoom != nZoom)
        {
            SetZoom(nZoom);
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
    else if (event.GetId() == MCCANVAS_REFRESH_TIMER_ID)
    {
        wxClientDC dc(this);
        DoPrepareDC(dc);

        DrawMousePos(&dc, m_pointLastMousePos.x, m_pointLastMousePos.y, m_nZoom);
        DrawMousePos(&dc, m_pointNextMousePos.x, m_pointNextMousePos.y, m_nZoom);
        m_pointLastMousePos = m_pointNextMousePos;
    }
}
