/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) 2003-2010 Thomas Giesel
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
#include "HiResBlock.h"
#include "HiResBitmap.h"

/*****************************************************************************/
HiResBlock::HiResBlock() :
    m_pParent(NULL)
{
    memset(m_aBitmap, 0, sizeof(m_aBitmap));
    m_c64Color[0].SetColor(MC_BLACK);
    m_c64Color[1].SetColor(MC_BLACK);
}


/*****************************************************************************/
void HiResBlock::SetIndexedColor(int index, C64Color col)
{
    if (index < 2)
        m_c64Color[index] = col;
}


/*****************************************************************************/
const C64Color* HiResBlock::GetIndexedColor(int index) const
{
    if (index < 2)
        return &m_c64Color[index];
    else
        return NULL;
}


/*****************************************************************************/
int HiResBlock::CountIndexedColor(int index) const
{
    int i, cnt = 0;

    for (i = 0; i < HIRESBLOCK_WIDTH * HIRESBLOCK_HEIGHT; ++i)
        if (m_aBitmap[0][i] == index) ++cnt;
    return cnt;
}


/*****************************************************************************/
void HiResBlock::SetBitmapPixel(unsigned x, unsigned y, int index)
{
    if ((x < HIRESBLOCK_WIDTH) && (y < HIRESBLOCK_HEIGHT) &&
        (index < 2))
    {
        m_aBitmap[y][x] = index;
    }
}

/*****************************************************************************/
void HiResBlock::SetBitmapRAM(unsigned y, unsigned char val)
{
    int x;

    if (y < HIRESBLOCK_HEIGHT)
    {
        for (x = 0; x < HIRESBLOCK_WIDTH; ++x)
        {
            SetBitmapPixel(x, y,
                ((val >> (HIRESBLOCK_WIDTH - x - 1)) & 1));
        }
    }
}

/*****************************************************************************/
unsigned char HiResBlock::GetBitmapRAM(unsigned y) const
{
    unsigned char v;
    int x;

    v = 0;
    for (x = 0; x < HIRESBLOCK_WIDTH; ++x)
    {
        v |= m_aBitmap[y][x] << (HIRESBLOCK_WIDTH - x - 1);
    }
    return v;
}


/*****************************************************************************
 * Setzt einen Pixel an x/y in der Farbe col. Gibt es schon alle Vordergrund-
 * farben, wird je nach "mode" verfahren.
 * IGNORE:        SetPixel schlaegt fehl und gibt false zurueck.
 * LEAST:         Die in diesem Block am wenigsten benutzte Farbe wird
 *                gegen die neue Farbe ersetzt. Dann wird der Pixel gesetzt.
 * FORCE:         Die des Pixels x/y wird in diesem Block mit der neuen Farbe
 *                ersetzt.
 *
 * Es koennen auch Farben mit festem Index gesetzt werden
 */
void HiResBlock::SetPixel(unsigned x, unsigned y,
                       const C64Color& col, MCDrawingMode mode)
{
    int i;

    // Set colors with fixed index first
    i = (int)(mode - MCDrawingModeIndex0);
    if (mode >= MCDrawingModeIndex0 && mode <= MCDrawingModeIndex1)
    {
        SetIndexedColor(i, col);
        m_aBitmap[y][x] = i;
        return;
    }
    else if (mode >= MCDrawingModeIndex2 && mode <= MCDrawingModeIndex3)
        return; // oops

    /* Die vorhandenen Farben durchgehen und nachsehen, ob geeignet */
    for (i = 0; i < 2; ++i)
    {
        if ((CountIndexedColor(i) != 0) && (m_c64Color[i] == col))
        {
            m_aBitmap[y][x] = i;
            return;
        }
    }
    /* Sonst eine unbenutzte Farbe belegen */
    for (i = 0; i < 2; ++i)
    {
        if (CountIndexedColor(i) == 0)
        {
            m_aBitmap[y][x] = i;
            m_c64Color[i] = col;
            return;
        }
    }
    /* Color Clash, je nach Modus weiterverfahren */
    switch (mode)
    {
    case MCDrawingModeIgnore:
        break;

    case MCDrawingModeForce:
        /* Diese Farbe ersetzen */
        i = m_aBitmap[y][x];
            m_c64Color[i] = col;
        break;

    case MCDrawingModeLeast:
        /* COLMODE_LEAST -> Am wenigsten benutzte Vordergrundfarbe nehmen */
        i = CountIndexedColor(0) < CountIndexedColor(1) ? 0 : 1;
        m_aBitmap[y][x] = i;
        m_c64Color[i] = col;

    default:
        break;
    }
}
