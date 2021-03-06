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

#include "ToolColorPicker.h"
#include "MCDoc.h"
#include "MCApp.h"
#include "PalettePanel.h"

ToolColorPicker::ToolColorPicker()
{
}

ToolColorPicker::~ToolColorPicker()
{
}

/*****************************************************************************/
/*
 * Return the ID of this tool.
 */
int ToolColorPicker::GetToolId()
{
    return MC_ID_TOOL_COLOR_PICKER;
}

/*****************************************************************************/
/*
 * Start the tool at the given coordinates (i.e. Mouse button down).
 *
 * Just draw a dot at the given coordinates.
 * X and y are bitmap coordinates.
 * bSecondaryFunction is true if the tool was invoked with a
 * secondary (i.e. right) mouse button.
 */
void ToolColorPicker::Start(int x, int y, bool bSecondaryFunction)
{
    int nColor;

    ToolBase::Start(x, y, bSecondaryFunction);

    nColor = m_pDoc->GetBitmap()->GetColor(x, y)->GetColor();

    wxGetApp().GetPalettePanel()->SelectColor(nColor, bSecondaryFunction);
}
