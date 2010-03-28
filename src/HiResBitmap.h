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

#ifndef HIRESBITMAP_H
#define HIRESBITMAP_H

#include "HiResBlock.h"
#include "ToolBase.h"
#include "BitmapBase.h"

#define HIRESBITMAP_BYTES_PER_BLOCK 8

#define HIRES_X 320
#define HIRES_Y 200

#define HIRESBITMAP_XBLOCKS (HIRES_X / 8)
#define HIRESBITMAP_YBLOCKS (HIRES_Y / 8)

class HiResBitmap: public BitmapBase
{
public:
    HiResBitmap(void);
    ~HiResBitmap(void);
    virtual BitmapBase* Copy() const;

    virtual int GetWidth() const;
    virtual int GetHeight() const;

    virtual int GetNIndexes() const;
    virtual const C64Color* GetColorByIndex(int x, int y, int index) const;
    virtual int CountColorByIndex(int x, int y, int index) const;

    virtual const C64Color* GetColor(int x, int y) const;
    virtual void SetPixel(int x, int y, const C64Color& col,
                          MCDrawingMode mode = MCDrawingModeIgnore);

    void SetBackground(C64Color col);
    unsigned char GetBackground() const;

    void SetScreenRAM(unsigned offset, unsigned char val);
    unsigned char GetScreenRAM(unsigned offset) const;

    void SetColorRAM(unsigned offset, unsigned char val);
    unsigned char GetColorRAM(unsigned offset) const;

    void SetBitmapRAM(unsigned offset, unsigned char val);
    unsigned char GetBitmapRAM(unsigned offset);

    const HiResBlock* GetHiResBlock(unsigned x, unsigned y) const;

    static const C64Color black;

protected:
    HiResBlock m_aHiResBlock[HIRESBITMAP_YBLOCKS][HIRESBITMAP_XBLOCKS];
};

#endif // HIRESBITMAP_H
