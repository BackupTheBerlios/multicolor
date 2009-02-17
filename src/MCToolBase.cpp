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

#include <wx/string.h>
#include <wx/msgdlg.h>

#include "MCToolBase.h"
#include "MCDoc.h"

MCToolBase::MCToolBase()
    : m_nColorPrimary(1)
    , m_nColorSecondary(0)
    , m_bSecondaryFunction(false)
    , m_xStart(0)
    , m_yStart(0)
    , m_drawingMode(MCDrawingModeLeast)
    , m_pDoc(NULL)
{
}

MCToolBase::~MCToolBase()
{
}

/*****************************************************************************/
/* 
 * Set the primary and secondary color for the tool before stating it.
 * 
 * Just remember the two colors in the object.
 * Note that not all tools will use both or even only one of these colors.
 * X and y are bitmap coordinates.
 */
void MCToolBase::SetColors(int nColorPrimary, int nColorSecondary)
{
    m_nColorPrimary   = nColorPrimary;
    m_nColorSecondary = nColorSecondary;
}

/*****************************************************************************/
/*
 * Start the tool at the given coordinates (i.e. Mouse button down).
 * 
 * The default implementation just saves the start point and creates a backup
 * of the documents bitmap.
 * 
 * X and y are bitmap coordinates.
 * bSecondaryFunction is true if the tool was invoked with a 
 * secondary (i.e. right) mouse button.
 */
void MCToolBase::Start(int x, int y, bool bSecondaryFunction)
{
    m_pDoc->m_bitmapToolCopy = m_pDoc->m_bitmap;

    m_xStart = x;
    m_yStart = y;
}

/*****************************************************************************/
/*
 * Mouse has been moved while the button was kept pressed.
 *
 * The default implementation does nothing.
 * X and y are bitmap coordinates.
 */
void MCToolBase::Move(int x, int y)
{
}

/*****************************************************************************/
/*
 * Finish the tool at the given coordinates (i.e. Mouse button up)
 *
 * The default implementation does nothing.
 * X and y are bitmap coordinates.
 */
void MCToolBase::End(int x, int y)
{
}
