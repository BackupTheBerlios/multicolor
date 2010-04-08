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
#include "FullDocRenderer.h"
#include "C64Color.h"

#define FIXP_SHIFT 16

/* Ein EWMA-Filter.
 * CONST ist der Exponent fuer die Filterkonstante 1 - 1/(2^CONST)
 */
#define PAINT_FILTER(F,X,CONST) ( (F)+=(X)-(F) - (((X)-(F))>>(CONST)) )


FullDocRenderer::FullDocRenderer():
    m_bEmulateTV(true),
    m_nZoom(1)
{
}

FullDocRenderer::~FullDocRenderer()
{
    //dtor
}


/*****************************************************************************/
/**
 * Set the document this renderer has to show from now. May be NULL if
 * there is no document attached.
 */
void FullDocRenderer::SetDoc(DocBase* pDoc)
{
    // make sure the image will be reallocated next time it must be drawn
    m_image.Destroy();
    DocRenderer::SetDoc(pDoc);
}


/*****************************************************************************/
/*
 * Enable/Disable TV emulation and delete the cache.
 */
void FullDocRenderer::SetEmulateTV(bool bTV)
{
    m_bEmulateTV = bTV;
    RedrawAll();
}


/*****************************************************************************/
/**
 * Set zoom factor and delete the cache.
 */
void FullDocRenderer::SetZoom(unsigned nZoom)
{
    m_nZoom = nZoom;
    OnZoomChanged();
    RedrawAll();
}


/*****************************************************************************/
/**
 * Call this to redraw the whole document in this renderer.
 */
void FullDocRenderer::RedrawAll()
{
    // make sure the image will be reallocated next time it must be drawn
    m_image.Destroy();

    DocRenderer::RedrawAll();
}


/******************************************************************************
 **
 * Is called when the zoom scale changed, e.g. through SetZoom.
 */
void FullDocRenderer::OnZoomChanged()
{
}


/******************************************************************************
 **
 * Paint the scaled bitmap into the cache image at scale 1:1 and 2:1.
 *
 * The caller must make sure that:
 * x1 <= x2, y1 <= y2, 0 <= x < w, 0 <= y <= h
 *
 * x1, y1, x2, y2 are in bitmap space
 */
void FullDocRenderer::DrawScaleSmall(wxDC* pDC,
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

    filter = aFilters[m_nZoom];

    if (!m_image.IsOk())
    {
        m_image.Create(
            pB->GetPixelXFactor() * pB->GetWidth() * m_nZoom,
            pB->GetPixelYFactor() * pB->GetHeight() * m_nZoom, false);
        // in this case we have to render the whole image
        x1 = 0;
        y1 = 0;
        x2 = pB->GetWidth() - 1;
        y2 = pB->GetHeight() - 1;
    }

    pPixels = m_image.GetData();
    nPitch  = m_image.GetWidth() * 3;
    xFactor = pB->GetPixelXFactor() * m_nZoom;
    w       = xFactor * pB->GetWidth();

    // We draw all full lines of the area so the blur has the right effect
    for (y =  y1 * pB->GetPixelYFactor();
         y <= y2 * pB->GetPixelYFactor() * m_nZoom;
         ++y)
    {
        fixr = fixg = fixb = 64 << FIXP_SHIFT;
        p = pPixels + y * nPitch;

        for (x = 0; x < w; ++x)
        {
            if (x % xFactor == 0)
            {
                col = pB->GetColor(x / xFactor, y / m_nZoom)->GetRGB();
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
