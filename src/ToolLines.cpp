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

#include <wx/gdicmn.h>

#include "ToolLines.h"
#include "MCDoc.h"
#include "MCApp.h"

ToolLines::ToolLines()
{
}

ToolLines::~ToolLines()
{
}

/*****************************************************************************/
/*
 * Return the ID of this tool.
 */
int ToolLines::GetToolId()
{
    return MC_ID_TOOL_LINES;
}

/*****************************************************************************/
/*
 * Start the tool at the given coordinates (i.e. Mouse button down).
 *
 * Fill the area around the given point.
 * X and y are bitmap coordinates.
 * bSecondaryFunction is true if the tool was invoked with a
 * secondary (i.e. right) mouse button.
 */
void ToolLines::Start(int x, int y, bool bSecondaryFunction)
{
    ToolBase::Start(x, y, bSecondaryFunction);

    // initialise the "previous" preview line end coordinates
    m_xOld = x;
    m_yOld = y;
}

/*****************************************************************************/
/**
 * Mouse has been moved while the button was kept pressed.
 *
 * Restory the documents bitmap from a saved one and draw a preview of the
 * line.
 *
 * X and y are bitmap coordinates.
 */
void ToolLines::Move(int x, int y)
{
    int x1, y1, x2, y2;

    m_pDoc->RestoreBitmap();
    m_pDoc->GetBitmap()->Line(m_xStart, m_yStart, x, y, m_nColorSelected,
            m_drawingMode);

    // redraw an area that includes the previous line preview and this line
    x1 = x2 = m_xStart;
    y1 = y2 = m_yStart;
    if (x < x1) x1 = x;
    if (x > x2) x2 = x;
    if (y < y1) y1 = y;
    if (y > y2) y2 = y;
    if (m_xOld < x1) x1 = m_xOld;
    if (m_xOld > x2) x2 = m_xOld;
    if (m_yOld < y1) y1 = m_yOld;
    if (m_yOld > y2) y2 = m_yOld;

    m_pDoc->Refresh(x1, y1, x2, y2); // ???

    m_xOld = x;
    m_yOld = y;
}

/*****************************************************************************/
/*
 * Finish the tool at the given coordinates (i.e. Mouse button up)
 *
 * Draw the line.
 * X and y are bitmap coordinates.
 */
void ToolLines::End(int x, int y)
{
    Move(x, y);
    m_pDoc->PrepareUndo();
}
