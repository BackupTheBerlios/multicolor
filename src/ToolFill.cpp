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

#include "ToolFill.h"
#include "MCDoc.h"
#include "MCApp.h"

ToolFill::ToolFill()
{
}

ToolFill::~ToolFill()
{
}


/*****************************************************************************/
/*
 * Return the ID of this tool.
 */
int ToolFill::GetToolId()
{
    return MC_ID_TOOL_FILL;
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
void ToolFill::Start(int x, int y, bool bSecondaryFunction)
{
    ToolBase::Start(x, y, bSecondaryFunction);

    m_pDoc->GetBitmap()->FloodFill(x, y, m_nColorSelected, m_drawingMode);
    m_pDoc->PrepareUndo();
}
