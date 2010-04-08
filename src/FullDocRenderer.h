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

#ifndef FULLDOCRENDERER_H
#define FULLDOCRENDERER_H

#include <wx/dc.h>
#include <wx/image.h>

#include "DocRenderer.h"

/*****************************************************************************/
/**
 * This subclass of DocRenderer adds methods and data to be able to render
 * the full size of the document.
 */
class FullDocRenderer : public DocRenderer
{
public:
    FullDocRenderer();
    virtual ~FullDocRenderer();

    virtual void SetDoc(DocBase* pDoc);

    void SetEmulateTV(bool bTV);
    bool GetEmulateTV();
    void SetZoom(unsigned nZoom);
    unsigned GetZoom();

    virtual void RedrawAll();
protected:
    /**
     * Is called when the zoom scale changed, e.g. through SetZoom.
     */
    virtual void OnZoomChanged();

    void DrawScaleSmall(wxDC* pDC,
            unsigned x1, unsigned y1, unsigned x2, unsigned y2);

    bool        m_bEmulateTV;
    unsigned    m_nZoom;

private:
    /// This image is used as cache at zoom levels 1:1 and 2:1
    wxImage     m_image;
};


/*****************************************************************************/
/*
 * Return the state of the TV emulation (on/off).
 */
inline bool FullDocRenderer::GetEmulateTV()
{
    return m_bEmulateTV;
}


/*****************************************************************************/
/*
 * Return the current zoom factor.
 */
inline unsigned FullDocRenderer::GetZoom()
{
    return m_nZoom;
}

#endif // FULLDOCRENDERER_H
