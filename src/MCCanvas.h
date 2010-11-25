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

#ifndef MCCANVAS_H
#define MCCANVAS_H

#include <list>
#include <wx/scrolwin.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/timer.h>
#include <wx/cursor.h>

#include "MCBitmap.h"
#include "DocRenderer.h"

#define MCCANVAS_SCROLL_TIMER_ID  1
#define MCCANVAS_REFRESH_TIMER_ID 2

// If we are closer as that many (screen) pixels to the border, we scroll
#define MCCANVAS_SCROLL_THRESHOLD 48

// This is the timeout before scrolling the first time [ms]
#define MCCANVAS_SCROLL_DELAY 750

// Scroll interval [ms]
#define MCCANVAS_SCROLL_INTERVAL 150


class ToolBase;
class DocBase;

class MCCanvas: public wxScrolledWindow, public DocRenderer
{
public:
    MCCanvas(wxWindow* pParent, int nStyle);

    virtual void RedrawDoc(int x1, int y1, int x2, int y2);
    virtual void OnDocMouseMoved(int x, int y);

    virtual void OnDraw(wxDC& dc);

    void SetDoc(DocBase* pDoc);

    void CenterBitmapPoint(int x, int y);

    static void UpdateAllCursorTypes();

    void SetEmulateTV(bool bTV);
    bool GetEmulateTV();
    void SetZoom(unsigned nZoom);
    unsigned GetZoom();

    void UpdateCursorType();

protected:
    static std::list<MCCanvas*> m_listCanvasInstances;

    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    } RGBPixel;

    void ToBitmapCoord(int* px, int* py, int x, int y, bool bScroll);
    void ToCanvasCoord(int* px, int* py, int x, int y);
    bool CheckScrolling(int xMouse, int yMouses);
    int CheckScrollingOneDirection(
            int nScroll, int nMousePos, int nAreaMax, int nScrollMax);
    void MoveCursorWithKey(int nKeyCode);
    void StartTool(int x, int y, bool bSecondary);
    void UpdateCursorPosition(int x, int y, bool bCanvasCoordinates);
    void EndTool(int x, int y);

    void OnButtonDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnButtonUp(wxMouseEvent& event);
    void OnMButtonDown(wxMouseEvent& event);
    void OnMButtonUp(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnEnter(wxMouseEvent& event);
    void OnLeave(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);

    bool        m_bEmulateTV;
    unsigned    m_nZoom;

    // Points to the currently active tool or NULL
    ToolBase* m_pActiveTool;

    // true if this is the small preview window
    bool        m_bPreviewWindow;

    // Position where the mouse has been drawn (bitmap coord), -1/-1 for none
    wxPoint     m_pointLastMousePos;

    // Position where the mouse will be drawn next (bitmap coord), -1/-1 for none
    wxPoint     m_pointNextMousePos;

    // When this timer is still running, auto scroll is disabled
    wxTimer     m_timerScrolling;

    wxTimer     m_timerRefresh;

    //// Drag Scrolling
    // true if we are dragging the image for scrolling
    bool        m_bDragScrollActive;

    // point (canvas coordinates) where the dragging started
    wxPoint     m_pointDragScrollStart;

    // xScroll where the drag scroll started
    int         m_xDragScrollStart;

    // yScroll where the drag scroll started
    int         m_yDragScrollStart;
    //
    ////

    // This is true if the Color Picker is activated temporarily (using Shift)
    bool        m_bColorPickerActive;

    wxCursor    m_cursorCloneBrush;
    wxCursor    m_cursorColorPicker;
    wxCursor    m_cursorDots;
    wxCursor    m_cursorFloodFill;
    wxCursor    m_cursorFreehand;
    wxCursor    m_cursorLines;
    wxCursor    m_cursorRect;
};


/*****************************************************************************/
/**
 * Return the state of the TV emulation (on/off).
 */
inline bool MCCanvas::GetEmulateTV()
{
    return m_bEmulateTV;
}


/*****************************************************************************/
/**
 * Return the current zoom factor.
 */
inline unsigned MCCanvas::GetZoom()
{
    return m_nZoom;
}

#endif /* MCCANVAS_H */
