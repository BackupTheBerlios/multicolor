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

DocRenderer::DocRenderer() :
    m_pDoc(NULL)
{
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

    RedrawAll();
}


/*****************************************************************************/
/**
 * Call this to redraw the whole document in this renderer.
 */
void DocRenderer::RedrawAll()
{
    if (m_pDoc)
    {
        OnDocChanged(
            0, 0,
            m_pDoc->GetBitmap()->GetWidth() - 1,
            m_pDoc->GetBitmap()->GetHeight() - 1);
    }
}
