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

#ifndef MCBITMAP_H
#define MCBITMAP_H

#include "MCBlock.h"
#include "MCToolBase.h"
#include "BitmapBase.h"

#define MCBITMAP_XBLOCKS 40
#define MCBITMAP_YBLOCKS 25
#define MCBITMAP_BYTES_PER_BLOCK 8

#define MC_X 160
#define MC_Y 200

class MCBitmap : public BitmapBase
{
public:
    MCBitmap(void);
    ~MCBitmap(void);

    virtual unsigned GetWidth() const;
    virtual unsigned GetHeight() const;
    virtual unsigned GetPixelXFactor() const;
    virtual const C64Color* GetColor(unsigned x, unsigned y) const;

    void SetBackground(C64Color col);
    unsigned char GetBackground() const;

    void SetScreenRAM(unsigned offset, unsigned char val);
    unsigned char GetScreenRAM(unsigned offset) const;

    void SetColorRAM(unsigned offset, unsigned char val);
    unsigned char GetColorRAM(unsigned offset) const;

    void SetBitmapRAM(unsigned offset, unsigned char val);
    unsigned char GetBitmapRAM(unsigned offset);

    const MCBlock* GetMCBlock(unsigned x, unsigned y) const;
    bool SetPixel(unsigned x, unsigned y, const C64Color& col,
            MCDrawingMode mode = MCDrawingModeIgnore);
    bool FloodFill(unsigned x, unsigned y, const C64Color& col,
            MCDrawingMode mode = MCDrawingModeIgnore);
    bool Line(int x1, int y1, int x2, int y2,
            const C64Color& col, MCDrawingMode mode);

    static const C64Color black;

protected:
    MCBlock m_aMCBlock[MCBITMAP_YBLOCKS][MCBITMAP_XBLOCKS];
};

#endif
