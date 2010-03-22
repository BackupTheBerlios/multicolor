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

#ifndef MCBLOCK_H
#define MCBLOCK_H

#include "C64Color.h"
#include "ToolBase.h"

#define MCBLOCK_WIDTH 4
#define MCBLOCK_HEIGHT 8

class MCBitmap;


class MCBlock
{
protected:
    C64Color m_c64Color[4];
    unsigned char m_aBitmap[MCBLOCK_HEIGHT][MCBLOCK_WIDTH];
    MCBitmap* m_pParent;

public:

    MCBlock();
    ~MCBlock(void);

    void SetParent(MCBitmap* pParent);

    void SetMCColor(int index, C64Color col);

    void SetBitmapPixel(unsigned x, unsigned y, int index);

    void SetBitmapRAM(unsigned y, unsigned char val);
    unsigned char GetBitmapRAM(unsigned y) const;

    const C64Color* GetMCColor(int index) const;
    int CountMCIndex(int index) const;
    bool SetPixel(unsigned x, unsigned y, const C64Color& col,
            MCDrawingMode mode = MCDrawingModeIgnore);

    const C64Color* GetColor(unsigned x, unsigned y) const;
};


inline const C64Color* MCBlock::GetColor(unsigned x, unsigned y) const
{
    return &(m_c64Color[m_aBitmap[y][x]]);
}

inline void MCBlock::SetParent(MCBitmap* pParent)
{
    m_pParent = pParent;
}

#endif
