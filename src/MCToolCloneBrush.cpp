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

#include "MCToolCloneBrush.h"
#include "MCDoc.h"
#include "MCChildFrame.h"
#include "MCApp.h"


/*****************************************************************************/
MCToolCloneBrush::MCToolCloneBrush() :
    m_pDocSource(NULL),
    m_pDocDest(NULL),
    m_dx(0),
    m_dy(0)
{
}


/*****************************************************************************/
MCToolCloneBrush::~MCToolCloneBrush()
{
}


/*****************************************************************************/
/*
 * Return the ID of this tool.
 */
int MCToolCloneBrush::GetToolId()
{
    return MC_ID_TOOL_CLONE_BRUSH;
}

/*****************************************************************************/
/*
 * Start the tool at the given coordinates (i.e. Mouse button down).
 *
 *
 *
 * X and y are bitmap coordinates.
 * bSecondaryFunction is true if the tool was invoked with a
 * secondary (i.e. right) mouse button.
 */
void MCToolCloneBrush::Start(int x, int y, bool bSecondaryFunction)
{
    MCToolBase::Start(x, y, bSecondaryFunction);

    if (bSecondaryFunction)
    {
        m_pDocSource = m_pDoc;
        m_xSource = x;
        m_ySource = y;
        m_pDocDest = NULL;
    }
    else
    {
        ClonePixel(x, y);
        m_pDoc->Refresh(x, y, x, y);
    }
}

/*****************************************************************************/
/*
 * Mouse has been moved while the button was kept pressed.
 *
 *
 * X and y are bitmap coordinates.
 */
void MCToolCloneBrush::Move(int x, int y)
{
    if (!m_bSecondaryFunction)
    {
        CloneLine(m_xStart, m_yStart, x, y);

        // prepare start point for next segment
        m_xStart = x;
        m_yStart = y;
        m_pDoc->Refresh(m_xStart, m_yStart, x, y);
    }
}

/*****************************************************************************/
/*
 * Finish the tool at the given coordinates (i.e. Mouse button up)
 *
 * X and y are bitmap coordinates.
 */
void MCToolCloneBrush::End(int x, int y)
{
    m_pDoc->PrepareUndo();
}

/*****************************************************************************
 *
 */
void MCToolCloneBrush::CloneLine(int x1, int y1, int x2, int y2)
{
    double fx, fy, step;
    int    tmp;

    if (abs(x2 - x1) > abs(y2 - y1))
    {
        // horizontal
        if (x2 < x1)
        {
            // swap start and end
            tmp = x1; x1 = x2; x2 = tmp;
            tmp = y1; y1 = y2; y2 = tmp;
        }

        fy = (double) y1 + 0.5;
        step = x2 == x1 ? 0.0 : (double)(y2 - y1) / (double)(x2 - x1);
        while (x1 <= x2)
        {
            ClonePixel(x1++, (unsigned) fy);
            fy += step;
        }
    }
    else
    {
        // vertical
        if (y2 < y1)
        {
            // swap start and end
            tmp = x1; x1 = x2; x2 = tmp;
            tmp = y1; y1 = y2; y2 = tmp;
        }

        fx = (double) x1 + 0.5;
        step = y2 == y1 ? 0.0 : (double)(x2 - x1) / (double)(y2 - y1);
        while (y1 <= y2)
        {
            ClonePixel((int) fx, y1++);
            fx += step;
        }
    }
}

/*****************************************************************************/
/*
 * Clone from source to destination.
 *
 * X and y are bitmap coordinates.
 */
void MCToolCloneBrush::ClonePixel(int x, int y)
{
    C64Color col;

    if (!m_pDocSource)
        return;

    // Is the document different from the last destination?
    if (m_pDoc != m_pDocDest)
    {
        // Then we clone to a new one
        m_pDocDest = m_pDoc;
        m_dx = x - m_xSource;
        m_dy = y - m_ySource;
    }

    col = *m_pDocSource->m_bitmap.GetColor(x - m_dx, y - m_dy);
    m_pDocDest->m_bitmap.SetPixel(x, y, col, m_drawingMode);
}
