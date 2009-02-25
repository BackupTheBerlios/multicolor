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

#include "MCApp.h"
#include "MCCanvas.h"
#include "MCDoc.h"
#include "PalettePanel.h"
#include "MCDrawingModePanel.h"
#include "MCToolBase.h"
#include "MCMainFrame.h"
#include "MCChildFrame.h"

#define FIXP_SHIFT 16

/* Ein EWMA-Filter.
 * CONST ist der Exponent fuer die Filterkonstante 1 - 1/(2^CONST)
 */
#define PAINT_FILTER(F,X,CONST) ( (F)+=(X)-(F) - (((X)-(F))>>(CONST)) )



/*****************************************************************************/
MCCanvas::MCCanvas(
    MCChildFrame* pFrame, wxWindow* pParent, int nStyle) :
    wxScrolledWindow(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, nStyle),
    m_pFrame(pFrame),
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

    // set min size incl. borders so the preview won't get scroll bars
    wxSize size(GetWindowBorderSize());
    size.IncBy(2 * MC_X * m_nScale, MC_Y * m_nScale);
    SetMinSize(size);
}

/*****************************************************************************/
MCCanvas::~MCCanvas()
{
}

/*****************************************************************************/
/*
 * Set the ChildFrame this view refers to.
 */
void MCCanvas::SetChildFrame(MCChildFrame* pFrame)
{
    m_pFrame = pFrame;
	DestroyCache();
    Refresh(false);
}

/*****************************************************************************/
/*
 * Enable/Disable TV emulation and delete the cache.
 */
void MCCanvas::SetEmulateTV(bool bTV)
{
    m_bEmulateTV = bTV;
    DestroyCache();
    Refresh(false);
}

/*****************************************************************************/
/*
 * Set zoom factor and delete the cache.
 */
void MCCanvas::SetScale(int nScale)
{
    m_nScale = nScale;
    DestroyCache();
    Refresh(false);
}

/*****************************************************************************/
void MCCanvas::OnDraw(wxDC& rDC)
{
    // create a cache if we don't have one
    if (!m_image.IsOk())
    {
        CreateCache();
    }

    if (m_pFrame)
    {
        MCDoc* pDoc = (MCDoc*) m_pFrame->GetDocument();
        Paint(&pDoc->m_bitmap);

        rDC.DrawBitmap(*(GetImage()), 0, 0, false);

        rDC.SetPen(*wxTRANSPARENT_PEN);
        rDC.SetBrush(*wxGREY_BRUSH);
        rDC.DrawRectangle(0, MC_Y * m_nScale, GetSize().GetWidth(),
                GetSize().GetHeight());
        rDC.DrawRectangle(2 * MC_X * m_nScale, 0, GetSize().GetWidth(),
                GetSize().GetHeight());

        DrawMousePos(&rDC);
	}
	else
	{
		// Simply draw a black box
        rDC.SetPen(*wxTRANSPARENT_PEN);
		rDC.SetBrush(*wxBLACK_BRUSH);
        rDC.DrawRectangle(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
	}
}

/******************************************************************************
 * Erzeugt die Bitmap.
 */
void MCCanvas::CreateCache()
{
    int x, y;
    unsigned char* pPixels;
    unsigned char* p;
    int            nPitch;

    m_image.Create(2 * MC_X * m_nScale, MC_Y * m_nScale, false);

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;

    for (y = 0; y < MC_Y * m_nScale; ++y)
    {
        p = pPixels + y * nPitch;
        for (x = 0; x < MC_X * 2 * m_nScale; ++x)
        {
            *p++ = 0x22;
            *p++ = 0x22;
            *p++ = 0x22;
        } /* x */
    } /* y */
    UpdateVirtualSize();
}

/******************************************************************************
 * Verwirft die Bitmap.
 */
void MCCanvas::DestroyCache(void)
{
   m_image.Destroy();
}

/*****************************************************************************/
// Erzeugt die skalierte Bitmaps
void MCCanvas::Paint(const MCBitmap* pMCB)
{
    if (m_nScale <= 2)
        PaintScaleSmall(pMCB);
    else if (m_nScale <= 4)
        PaintScaleMedium(pMCB);
    else
        PaintScaleBig(pMCB);
}

/******************************************************************************
 * Zeichnet das Bild bei Skalierung 1:1 bis 2:1.
 */
void MCCanvas::PaintScaleSmall(const MCBitmap* pMCB)
{
//    wxDC      dc;
    unsigned long col;
    int      fixr, fixg, fixb, tmpr, tmpg, tmpb;
    const int aFilters[] = {0, 2, 1};
    int      filter;
    int      x, y;
    unsigned char* pPixels;
    unsigned char* p;
    int            nPitch;

    filter = aFilters[m_nScale];

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;

    for (y = 0; y < MC_Y * m_nScale; ++y)
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
}

/******************************************************************************
 * Zeichnet das Bild bei Skalierung 4:1
 */
void MCCanvas::PaintScaleMedium(const MCBitmap* pMCB)
{
    int            x, y;
    unsigned char* pPixels;
    int            nPitch;
    unsigned long  col;
    wxRect         rect;

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;

    for (y = 0; y < MC_Y; ++y)
    {
        for (x = 0; x < MC_X; ++x)
        {
            rect.x      = 2 * m_nScale * x;
            rect.width  = 2 * m_nScale - 1;
            rect.y      = m_nScale * y;
            rect.height = m_nScale - 1;
            col = pMCB->GetColor(x, y)->GetRGB();

            FillRectangle(pPixels, nPitch, &rect, col);
        }
    }

    for (y = 0; y < 200 * m_nScale; y += 8 * m_nScale)
    {
        for (x = 0; x < 320 * m_nScale; x += 8 * m_nScale)
        {
            col = pMCB->GetColor(
                x / (2 * m_nScale), y / m_nScale)->GetContrastRGB();

            rect.x      = x - 1;
            rect.width  = 1;
            rect.y      = y - 1;
            rect.height = 1;

            FillRectangle(pPixels, nPitch, &rect, col);
        }
    }
}


/******************************************************************************
 * Zeichnet das Bild bei Skalierung > 4:1.
 */
void MCCanvas::PaintScaleBig(const MCBitmap* pMCB)
{
    unsigned char* pPixels;
    int            nPitch;
    unsigned long  col;
    wxRect         rect;
    int      x, y;

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;

    // Draw pixels
    for (y = 0; y < 200; ++y)
    {
        for (x = 0; x < 160; ++x)
        {
            rect.x      = 2 * m_nScale * x;
            rect.width  = 2 * m_nScale - 1;
            rect.y      = m_nScale * y;
            rect.height = m_nScale - 1;
            FillRectangle(pPixels, nPitch, &rect, pMCB->GetColor(x, y)->GetRGB());
        }
    }

    // Draw grid
    for (y = 0; y < 200 * m_nScale; y += 8 * m_nScale)
    {
        for (x = 0; x < 320 * m_nScale; x += 8 * m_nScale)
        {
            col = pMCB->GetColor(
                x / (2 * m_nScale), y / m_nScale)->GetContrastRGB();

            rect.x      = x - 6;
            rect.width  = 10;
            rect.y      = y - 1;
            rect.height = 1;
            FillRectangle(pPixels, nPitch, &rect, col);

            rect.x      = x -  1;
            rect.width  = 1;
            rect.y      = y - 6;
            rect.height = 10;
            FillRectangle(pPixels, nPitch, &rect, col);
        }
    }
}


/******************************************************************************
 * Zeichnet ein gefuelltes Rechteck in das Image.
 * pRect wird ggfs durch das clippen geaendert!
 */
void MCCanvas::FillRectangle(unsigned char* pPixels, int nPitch, wxRect* pRect, int rgb)
{
    unsigned char r = (unsigned char)rgb;
    unsigned char g = (unsigned char)(rgb >> 8);
    unsigned char b = (unsigned char)(rgb >> 16);
    int w, y;
    unsigned char* p;

    if (pRect->x < 0)
        pRect->x = 0;

    if (pRect->x >= m_image.GetWidth())
        pRect->x = m_image.GetWidth() - 1;

    if (pRect->width >= m_image.GetWidth() - pRect->x)
        pRect->width = m_image.GetWidth() - pRect->x - 1;

    if (pRect->y < 0)
        pRect->y = 0;

    if (pRect->y >= m_image.GetHeight())
        pRect->y = m_image.GetHeight() - 1;

    if (pRect->height >= m_image.GetHeight() - pRect->y)
            pRect->height = m_image.GetHeight() - pRect->y - 1;

    for (y = pRect->height - 1; y >= 0; y--)
    {
        p = pPixels + (y + pRect->y) * nPitch + pRect->x * 3;
        w = pRect->width;
        while(w--)
        {
            *p++ = b;
            *p++ = g;
            *p++ = r;
        }
    }
}

/******************************************************************************
 * Convert window point to bitmap coordinates. Scroll position and zoom are
 * taken into account.
 */
void MCCanvas::ToBitmapCoord(int* px, int* py, int x, int y)
{
    CalcUnscrolledPosition(x, y, px, py);

    *px /= (2 * m_nScale);
    *py /= m_nScale;

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
}


/******************************************************************************/
/*
 * Move the mouse pointer, update the view.
 */
void MCCanvas::SetMousePos(int x, int y)
{
    InvalidateMouseRect();
    m_pointLastMousePos.x = x;
    m_pointLastMousePos.y = y;
    InvalidateMouseRect();
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
    Refresh(false);
}

/******************************************************************************/
/*
 * Draw the mouse position or remove the drawing.
 */
void MCCanvas::DrawMousePos(wxDC* pDC)
{
    int x, y;

    x = m_pointLastMousePos.x;
    y = m_pointLastMousePos.y;
    ToCanvasCoord(&x, &y, x, y);

    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    pDC->SetPen(*wxWHITE_PEN);

    if (m_nScale >= 4)
    {
        pDC->DrawRectangle(x, y, 2 * m_nScale, m_nScale);
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
    //Refresh();
}


/*****************************************************************************/
void MCCanvas::OnButtonDown(wxMouseEvent& event)
{
    int x, y;

    if (m_pFrame)
    {
        MCDoc* pDoc = (MCDoc*) m_pFrame->GetDocument();
        MCDrawingMode mode = MCDrawingModeLeast;

        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());

        m_pActiveTool = wxGetApp().GetActiveDrawingTool();

        if (m_pActiveTool)
        {
            m_pActiveTool->SetColors(wxGetApp().GetPalettePanel()->GetColorA(),
                    wxGetApp().GetPalettePanel()->GetColorB());
            m_pActiveTool->SetDoc(pDoc);

            m_pActiveTool->SetDrawingMode(
                    wxGetApp().GetMainFrame()->GetToolPanel()->GetDrawingModePanel()->GetDrawingMode());

            m_pActiveTool->Start(x, y, event.GetButton() == wxMOUSE_BTN_RIGHT);
        }
    }
}


/*****************************************************************************/
void MCCanvas::OnButtonUp(wxMouseEvent& event)
{
    int x, y;

    if (m_pFrame)
    {
        MCDoc* pDoc = (MCDoc*) m_pFrame->GetDocument();

        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());

        if (m_pActiveTool)
        {
            m_pActiveTool->End(x, y);
            m_pActiveTool = NULL;
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

    if (m_pFrame)
    {
        ToBitmapCoord(&x, &y, event.GetX(), event.GetY());
        m_pFrame->SetMousePos(x, y);

        if (m_pActiveTool)
        {
            m_pActiveTool->Move(x, y);
        }

        x = event.GetX();
        y = event.GetY();
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
