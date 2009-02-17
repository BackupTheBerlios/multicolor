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

#include "MCToolFreehand.h"
#include "MCDoc.h"
#include "MCApp.h"

MCToolFreehand::MCToolFreehand()
{
}

MCToolFreehand::~MCToolFreehand()
{
}

/*****************************************************************************/
/*
 * Mouse has been moved while the button was kept pressed.
 *
 * Draw a line segment.
 * X and y are bitmap coordinates.
 */
void MCToolFreehand::Move(int x, int y)
{
    m_pDoc->m_bitmap.Line(m_xStart, m_yStart, x, y,
            m_nColorPrimary, m_drawingMode);

    // prepare start point for next segment
    m_xStart = x;
    m_yStart = y;

    m_pDoc->Refresh();
}

/*****************************************************************************/
/*
 * Finish the tool at the given coordinates (i.e. Mouse button up)
 *
 * Remember the current state for undo.
 * X and y are bitmap coordinates.
 */
void MCToolFreehand::End(int x, int y)
{
    m_pDoc->PrepareUndo();
}

/*****************************************************************************/
/*
 * Return the ID of this tool.
 */
int MCToolFreehand::GetToolId()
{
    return MC_ID_TOOL_FREEHAND;
}
