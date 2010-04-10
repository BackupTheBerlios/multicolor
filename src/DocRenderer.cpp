/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) Thomas Giesel
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

#include "BitmapBase.h"
#include "DocRenderer.h"
#include "C64Color.h"

#define FIXP_SHIFT 16

/* Ein EWMA-Filter.
 * CONST ist der Exponent fuer die Filterkonstante 1 - 1/(2^CONST)
 */
#define PAINT_FILTER(F,X,CONST) ( (F)+=(X)-(F) - (((X)-(F))>>(CONST)) )



/*****************************************************************************/
DocRenderer::DocRenderer() :
    m_pDoc(NULL)
{
}



/*****************************************************************************/
DocRenderer::~DocRenderer()
{
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);
}


/*****************************************************************************/
/**
 * Set the document this renderer has to show from now. May be NULL if
 * there is no document attached.
 */
void DocRenderer::SetDoc(DocBase* pDoc)
{
    // remove me from the previous document
    if (m_pDoc)
        m_pDoc->RemoveRenderer(this);

    m_pDoc = pDoc;

    // add me to the new document
    if (m_pDoc)
        m_pDoc->AddRenderer(this);

    RedrawDoc(0, 0, 9999, 9999);
}


/******************************************************************************/
/**
 * Draw the mouse position or remove the drawing.
 */
void DocRenderer::DrawMousePos(wxDC* pDC, int x, int y, unsigned nZoom)
{
    int xFactor, yFactor, c;
    int oldFn;
    BitmapBase* pB;

    if (!m_pDoc)
        return;

    pB = m_pDoc->GetBitmap();
    xFactor = pB->GetPixelXFactor() * nZoom;
    yFactor = pB->GetPixelYFactor() * nZoom;

    x *= xFactor;
    y *= yFactor;

    oldFn = pDC->GetLogicalFunction();
    pDC->SetLogicalFunction(wxXOR);
    pDC->SetBrush(*wxTRANSPARENT_BRUSH);
    pDC->SetPen(*wxWHITE_PEN);

    if (nZoom >= 4)
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

    pDC->SetLogicalFunction(oldFn);
}



/*****************************************************************************/
/**
 * Paint the scaled bitmap into the cache image at scale 1:1 and 2:1.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < w, 0 <= y <= h
 *
 * x1, y1, x2, y2 are in bitmap space
 */
void DocRenderer::DrawScaleSmall(wxDC* pDC, unsigned nZoom, bool bEmulateTV,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const BitmapBase* pB = m_pDoc->GetBitmap();
    MC_RGB   col;
    int      fixr, fixg, fixb, tmpr, tmpg, tmpb;
    const int aFilters[] = {0, 2, 1};
    int      filter;
    unsigned        x, y, w, yStop, xFactor, yFactor;
    unsigned char*  pPixels;
    unsigned char*  p;
    unsigned        nPitch;

    filter  = aFilters[nZoom];
    xFactor = pB->GetPixelXFactor() * nZoom;
    yFactor = pB->GetPixelYFactor() * nZoom;

    if (!m_image.IsOk() ||
        m_image.GetWidth() != pB->GetWidth() * xFactor ||
        m_image.GetHeight() != pB->GetHeight() * yFactor)
    {
        m_image.Create(pB->GetWidth() * xFactor,
                       pB->GetHeight() * yFactor, false);
        // in this case we have to render the whole image
        x1 = 0;
        y1 = 0;
        x2 = pB->GetWidth() - 1;
        y2 = pB->GetHeight() - 1;
    }

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;
    w       = xFactor * pB->GetWidth();

    // We draw all full lines of the area so the blur has the right effect
    yStop = (y2 + 1) * yFactor;
    for (y = y1 * yFactor; y < yStop; ++y) // todo: bug?
    {
        fixr = fixg = fixb = 64 << FIXP_SHIFT;
        p = pPixels + y * nPitch;

        for (x = 0; x < w; ++x)
        {
            if (x % xFactor == 0)
            {
                col = pB->GetColor(x / xFactor, y / nZoom)->GetRGB();
                tmpr = (col & 0xff) << FIXP_SHIFT;
                tmpg = ((col >> 8) & 0xff) << FIXP_SHIFT;
                tmpb = ((col >> 16) & 0xff) << FIXP_SHIFT;
            }
            if (bEmulateTV)
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


/*****************************************************************************/
/**
 * Draw the scaled bitmap at scale 4:1 and higher.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < w, 0 <= y <= h
 *
 * x1, y1, x2, y2 are in bitmap space
 */
void DocRenderer::DrawScaleBig(wxDC* pDC, unsigned nZoom,
        unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
    const BitmapBase* pB = m_pDoc->GetBitmap();
    wxBrush        brush(*wxBLACK);
    wxPen          pen(*wxBLACK);
    MC_RGB         rgb;
    wxRect         rect;
    unsigned       x, y, i, xFactor;

    xFactor = pB->GetPixelXFactor() * nZoom;

    pen.SetColour(MC_GRID_COL_R, MC_GRID_COL_G, MC_GRID_COL_B);

    // Draw blocks for pixels
    pDC->SetPen(*wxTRANSPARENT_PEN);
    rect.height = nZoom - 1;
    rect.width  = xFactor - 1;
    for (y = y1; y <= y2; ++y)
    {
        rect.y = nZoom * y + 1;
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
        pDC->DrawLine(i, nZoom * y1, i, nZoom * (y2 + 1));
    }
    for (y = y1; y <= y2; ++y)
    {
        i = nZoom * y;
        pDC->DrawLine(xFactor * x1, i, xFactor * (x2 + 1), i);
    }

    // Draw the cell grid, we extend the area to the next border at the
    // upper left corner
    x1 -= x1 % pB->GetCellWidth();
    x1 *= xFactor;
    y1 -= y1 % pB->GetCellHeight();
    y1 *= nZoom;

    x2 *= xFactor;
    y2 *= nZoom;
    for (y = y1; y <= y2; y += 8 * nZoom)
    {
        for (x = x1; x < x2; x += 8 * nZoom)
        {
            rgb = pB->GetColor(
                x / xFactor, y / nZoom)->GetContrastRGB();
            pen.SetColour(MC_RGB_R(rgb), MC_RGB_G(rgb), MC_RGB_B(rgb));
            pDC->SetPen(pen);

            pDC->DrawLine(x - nZoom / 2, y, x + nZoom / 2, y);
            pDC->DrawLine(x, y - nZoom / 2, x, y + nZoom / 2);
        }
    }
}
