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

#ifndef HIRESBLOCK_H
#define HIRESBLOCK_H

#include "C64Color.h"
#include "ToolBase.h"

#define HIRESBLOCK_WIDTH 8
#define HIRESBLOCK_HEIGHT 8

class HiResBitmap;

class HiResBlock
{
public:
    HiResBlock();

    void SetParent(HiResBitmap* pParent);

    void SetIndexedColor(int index, C64Color col);
    const C64Color* GetIndexedColor(int index) const;
    int CountIndexedColor(int index) const;

    void SetBitmapPixel(unsigned x, unsigned y, int index);
    void SetBitmapRAM(unsigned y, unsigned char val);
    unsigned char GetBitmapRAM(unsigned y) const;

    void SetPixel(unsigned x, unsigned y, const C64Color& col,
            MCDrawingMode mode = MCDrawingModeIgnore);
    const C64Color* GetPixel(unsigned x, unsigned y) const;

protected:
    C64Color m_c64Color[2];
    unsigned char m_aBitmap[HIRESBLOCK_HEIGHT][HIRESBLOCK_WIDTH];
    HiResBitmap* m_pParent;
};

/*****************************************************************************/
/**
 * Return the color of the given position. The caller must make sure that
 * the coordinates are valid.
 */
inline const C64Color* HiResBlock::GetPixel(unsigned x, unsigned y) const
{
    return &(m_c64Color[m_aBitmap[y][x]]);
}

/*****************************************************************************/
/**
 * Set the owner of this block.
 */
inline void HiResBlock::SetParent(HiResBitmap* pParent)
{
    m_pParent = pParent;
}


#endif // HIRESBLOCK_H
