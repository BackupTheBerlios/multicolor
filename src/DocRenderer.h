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

#ifndef DOCRENDERER_H_
#define DOCRENDERER_H_

#include <wx/dc.h>
#include <wx/image.h>

#include "DocBase.h"

/*****************************************************************************/
/**
 * This abstract class defines an interface for Classes which want to be
 * notified if a document has changed.
 *
 * Objects implementing this type can register themselfes to get updates
 * when the document view has to be updated.
 */
class DocRenderer
{
public:
    DocRenderer();

    /**
     * This is called when the document contents has changed, the parameters
     * report the area to be updated. Coordinates are in bitmap space.
     * x1/y1 is the upper left corner, x2/y2 is the bottom right corner.
     * It may by possible that x1 == x2 or y1 == y2 and it may be larger than
     * the actual image.
     */
    virtual void RedrawDoc(int x1, int y1, int x2, int y2) = 0;

    /**
     * This is called when the mouse has been moved in one of the views.
     * Coordinates are in bitmap space.
     */
    virtual void OnDocMouseMoved(int x, int y) = 0;

    /**
     * Set the document this renderer has to show from now. May be NULL if
     * there is no document attached.
     */
    virtual void SetDoc(DocBase* pDoc);

    DocBase* GetDoc();

protected:

    void DrawMousePos(wxDC* pDC, int x, int y, unsigned nZoom);

    void DrawScaleSmall(wxDC* pDC, unsigned nZoom, bool bEmulateTV,
            unsigned x1, unsigned y1, unsigned x2, unsigned y2);

    void DrawScaleBig(wxDC* pDC, unsigned nZoom,
            unsigned x1, unsigned y1, unsigned x2, unsigned y2);

    // Pointer to Document to be rendered or NULL
    DocBase*    m_pDoc;

private:
    /// This image is used as cache at zoom levels 1:1 and 2:1
    wxImage     m_image;
};


/*****************************************************************************/
/**
 * Return the pointer to the document related to this Renderer.
 */
inline DocBase* DocRenderer::GetDoc()
{
    return m_pDoc;
}

#endif /*DOCRENDERER_H_*/
