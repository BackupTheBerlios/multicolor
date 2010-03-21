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

#include <string.h>
#include "MCBlock.h"
#include "MCBitmap.h"

/*****************************************************************************/
MCBlock::MCBlock() :
    m_pParent(NULL)
{
    memset(m_aBitmap, 0, sizeof(m_aBitmap));
    m_c64Color[0].SetColor(MC_BLACK);
    m_c64Color[1].SetColor(MC_BLACK);
    m_c64Color[2].SetColor(MC_BLACK);
    m_c64Color[3].SetColor(MC_BLACK);
}

/*****************************************************************************/
MCBlock::~MCBlock(void)
{
}

/*****************************************************************************/
void MCBlock::SetMCColor(int index, C64Color col)
{
    if (index < 4)
        m_c64Color[index] = col;
}

/*****************************************************************************/
const C64Color* MCBlock::GetMCColor(int index) const
{
    if (index < 4)
        return &m_c64Color[index];
    else
        return NULL;
}


/*****************************************************************************/
void MCBlock::SetBitmapPixel(unsigned x, unsigned y, int index)
{
    if ((x < MCBLOCK_WIDTH) && (y < MCBLOCK_HEIGHT) &&
        (index < 4))
    {
        m_aBitmap[y][x] = index;
    }
}

/*****************************************************************************/
void MCBlock::SetBitmapRAM(unsigned y, unsigned char val)
{
    int x;

    if (y < MCBLOCK_HEIGHT)
    {
        for (x = 0; x < MCBLOCK_WIDTH; ++x)
        {
            SetBitmapPixel(x, y,
                ((val >> (2 * (MCBLOCK_WIDTH - x - 1))) & 0x03));
        }
    }
}

/*****************************************************************************/
unsigned char MCBlock::GetBitmapRAM(unsigned y) const
{
    unsigned char v;
    int x;

    //ASSERT (y < MCBLOCK_HEIGHT);

    v = 0;
    for (x = 0; x < MCBLOCK_WIDTH; ++x)
    {
        v |= m_aBitmap[y][x] << 2 * (MCBLOCK_WIDTH - x - 1);
    }
    return v;
}

/*****************************************************************************/

int MCBlock::CountMCIndex(int index) const
{
    int i, cnt = 0;

    for (i = 0; i < 4 * 8; ++i)
        if (m_aBitmap[0][i] == index) ++cnt;
    return cnt;
}

/*****************************************************************************
 * Setzt einen Pixel an x/y in der Farbe col. Gibt es schon 3 Vordergrund-
 * farben, wird je nach "mode" verfahren.
 * IGNORE:        SetPixel schlaegt fehl und gibt false zurueck.
 * LEAST:         Die in diesem Block am wenigsten benutzte Farbe wird
 *                gegen die neue Farbe ersetzt. Dann wird der Pixel gesetzt.
 * FORCE:         Die des Pixels x/y wird in diesem Block mit der neuen Farbe
 *                ersetzt.
 *
 * Es koennen auch Farben mit festem Index gesetzt werden, 0..3
 */
bool MCBlock::SetPixel(unsigned x, unsigned y,
                       const C64Color& col, MCDrawingMode mode)
{
    int i;

    // Set colors with fixed index first
    i = (int)(mode - MCDrawingModeIndex0);
    if (mode >= MCDrawingModeIndex0 && mode <= MCDrawingModeIndex3)
    {
        if (i == 0 && m_pParent)
            m_pParent->SetBackground(col);
        else
            SetMCColor(i, col);

        m_aBitmap[y][x] = i;
        return true;
    }

    /* Die 4 Farben durchgehen und nachsehen, ob geeignet */
    /* HG-Farbe immer verwenden, wenn moeglich */
    if (m_c64Color[0] == col)
    {
        m_aBitmap[y][x] = 0;
        return true;
    }
    /* Dann alle benutzte Farben durchgehen */
    for (i = 1; i < 4; ++i)
    {
        if ((CountMCIndex(i) != 0) && (m_c64Color[i] == col))
        {
            m_aBitmap[y][x] = i;
            return true;
        }
    }
    /* Sonst eine unbenutzte Farbe belegen */
    for (i = 1; i < 4; ++i)
    {
        if (CountMCIndex(i) == 0)
        {
            m_aBitmap[y][x] = i;
            m_c64Color[i] = col;
            return true;
        }
    }
    /* Color Clash, je nach Modus weiterverfahren */
    if (mode == MCDrawingModeIgnore)
        return false;

    if (mode == MCDrawingModeForce)
    {
        /* Diese Farbe ersetzen */
        i = m_aBitmap[y][x];
        if (i != 0)
            m_c64Color[i] = col;
        else
        {
            // Sonderfall: Die Hintergrundfarbe ist fest, hier muessen wir auf
            // den Modus Least Used umsteigen.
            mode = MCDrawingModeLeast;
        }
    }

    if (mode == MCDrawingModeLeast)
    {
        /* COLMODE_LEAST -> Am wenigsten benutzte Vordergrundfarbe nehmen */
        i = 1;
        if (CountMCIndex(2) < CountMCIndex(1))
            i = 2;
        if (CountMCIndex(3) < CountMCIndex(1))
            i = 3;
        m_aBitmap[y][x] = i;
        m_c64Color[i] = col;
    }
    return true;
}
