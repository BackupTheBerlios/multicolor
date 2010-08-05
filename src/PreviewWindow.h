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

#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <list>
#include <wx/scrolwin.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/timer.h>
#include <wx/cursor.h>

#include "MCBitmap.h"
#include "DocRenderer.h"

#define PREVIEWWINDOW_SCROLL_TIMER_ID  1
#define PREVIEWWINDOW_REFRESH_TIMER_ID 2

class ToolBase;
class DocBase;

class PreviewWindow: public wxWindow, public DocRenderer
{
public:
    PreviewWindow(wxWindow* pParent, int nStyle);

    virtual void RedrawDoc(int x1, int y1, int x2, int y2);
    virtual void OnDocMouseMoved(int x, int y);

    void OnPaint(wxPaintEvent& event);

    void SetEmulateTV(bool bTV);
    bool GetEmulateTV();

protected:
    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    } RGBPixel;

    void ToBitmapCoord(int* px, int* py, int x, int y);
    void ToCanvasCoord(int* px, int* py, int x, int y);
    void UpdateCursorPosition(int x, int y, bool bCanvasCoordinates);
    void UpdateCursorType();

    void OnButtonDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnButtonUp(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnEnter(wxMouseEvent& event);
    void OnLeave(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);

    bool        m_bEmulateTV;

    // Position where the mouse has been drawn (bitmap coord), -1/-1 for none
    wxPoint     m_pointLastMousePos;

    // Position where the mouse will be drawn next (bitmap coord), -1/-1 for none
    wxPoint     m_pointNextMousePos;

    wxTimer     m_timerRefresh;

    // This is true if the Color Picker is activated temporarily (using Shift)
    bool        m_bColorPickerActive;
};


/*****************************************************************************/
/**
 * Return the state of the TV emulation (on/off).
 */
inline bool PreviewWindow::GetEmulateTV()
{
    return m_bEmulateTV;
}


#endif /* PREVIEWWINDOW_H */
