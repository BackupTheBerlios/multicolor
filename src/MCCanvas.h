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

#include <wx/docview.h>
#include <wx/scrolwin.h>
#include <wx/image.h>
#include <wx/gdicmn.h>

#include "MCBitmap.h"

class MCToolBase;
class MCChildFrame;

class MCCanvas: public wxScrolledWindow
{
public:
    MCCanvas(MCChildFrame* pFrame, wxWindow* pParent, int nStyle);
    virtual ~MCCanvas(void);

    virtual void OnDraw(wxDC& dc);

    void SetChildFrame(MCChildFrame* pFrame);

    void OnButtonDown(wxMouseEvent& event);
    void OnButtonUp(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
	void OnEraseBackground(wxEraseEvent& event);

    void CreateCache();
    void DestroyCache();
    void SetEmulateTV(bool bTV);
    void SetScale(int nScale);
    void Paint(const MCBitmap* pMCB);
    void InvalidateMouseRect();

    wxImage* GetImage(void) { return &m_image; };

protected:

    // Pointer to MCChildFrame object owning this canvas
    MCChildFrame *m_pFrame;

    typedef struct
    {
        unsigned char b;
        unsigned char g;
        unsigned char r;
    } RGBPixel;

    void PaintScaleSmall(const MCBitmap* pMCB);
    void PaintScaleMedium(const MCBitmap* pMCB);
    void PaintScaleBig(const MCBitmap* pMCB);
    void FillRectangle(unsigned char* pPixels, int nPitch, wxRect* pRect, int rgb);

    void ToBitmapCoord(int* px, int* py, int x, int y);
    void ToCanvasCoord(int* px, int* py, int x, int y);
    void DrawMousePos(wxDC* pDC);
    void UpdateVirtualSize();

    // Points to the currently active tool or NULL
    MCToolBase* m_pActiveTool;

    bool      m_bEmulateTV;
    int       m_nScale;
    wxImage   m_image;
};


#endif /* MCCANVAS_H */
