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

#include <wx/scrolwin.h>
#include <wx/image.h>
#include <wx/gdicmn.h>
#include <wx/timer.h>

#include "MCBitmap.h"
#include "DocRenderer.h"

#define MCCANVAS_SCROLL_TIMER_ID 1

// If we are closer as that many (screen) pixels to the border, we scroll
#define MCCANVAS_SCROLL_THRESHOLD 40

// This is the timeout before scrolling the first time [ms]
#define MCCANVAS_SCROLL_DELAY 750

// Scroll interval [ms]
#define MCCANVAS_SCROLL_INTERVAL 150


class MCToolBase;
class MCDoc;

class MCCanvas: public wxScrolledWindow, public DocRenderer
{
public:
    MCCanvas(wxWindow* pParent, int nStyle);
    virtual ~MCCanvas(void);

    virtual void OnDocChanged(int x1, int y1, int x2, int y2);
    virtual void OnDocMouseMoved(int x, int y);
    virtual void OnDocDestroy(MCDoc* pDoc);

    virtual void OnDraw(wxDC& dc);

    void SetDoc(MCDoc* pDoc);
    MCDoc* GetDoc();

    void SetEmulateTV(bool bTV);
    bool GetEmulateTV();
    void SetScale(unsigned nScale);
    unsigned GetScale();

protected:

    // Pointer to Document to be rendered or NULL
    MCDoc* m_pDoc;

    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    } RGBPixel;

    void InvalidateMouseRect();

    void DrawScaleSmall(wxDC* pDC,
            unsigned x1, unsigned y1, unsigned x2, unsigned y2);
    void DrawScaleBig(wxDC* pDC,
            unsigned x1, unsigned y1, unsigned x2, unsigned y2);

    void ToBitmapCoord(int* px, int* py, int x, int y);
    void ToCanvasCoord(int* px, int* py, int x, int y);
    void DrawMousePos(wxDC* pDC);
    void UpdateVirtualSize();
    bool CheckScrolling(int xMouse, int yMouses);
    void UpdateMousePosition(int xMouse, int yMouse);

    void OnButtonDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnButtonUp(wxMouseEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnTimer(wxTimerEvent& event);

    void FixCoordinates(int* px1, int* py1,
            int* px2, int* py2);

    // Points to the currently active tool or NULL
    MCToolBase* m_pActiveTool;

    bool        m_bEmulateTV;
    unsigned    m_nScale;

    // Position where the mouse has been drawn (bitmap coord), -1/-1 for none
    wxPoint     m_pointLastMousePos;

    // This image is used as cache at zoom levels 1:1 and 2:1
    wxImage     m_image;

    // When this timer is still running, auto scroll is disabled
    wxTimer     m_timerScrolling;
};


/*****************************************************************************/
/*
 * Return the state of the TV emulation (on/off).
 */
inline bool MCCanvas::GetEmulateTV()
{
    return m_bEmulateTV;
}


/*****************************************************************************/
/*
 * Return the current zoom factor.
 */
inline unsigned MCCanvas::GetScale()
{
    return m_nScale;
}


/*****************************************************************************/
/*
 * Return the pointer to the document related to this Canvas.
 */
inline MCDoc* MCCanvas::GetDoc()
{
    return m_pDoc;
}


#endif /* MCCANVAS_H */
