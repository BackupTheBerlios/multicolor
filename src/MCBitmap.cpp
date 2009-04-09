/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) 2003-2009 Thomas Giesel
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

#include <stdlib.h>
#include <string.h>
#include "MCBitmap.h"
#include "MCToolBase.h"


const C64Color MCBitmap::black;


/*****************************************************************************/
MCBitmap::MCBitmap(void)
{
}

/*****************************************************************************/
MCBitmap::~MCBitmap(void)
{
}


/******************************************************************************/
/**
 * Return the width of this image.
 */
unsigned MCBitmap::GetWidth() const
{
    return MC_X;
}


/******************************************************************************/
/**
 * Return the height of this image.
 */
unsigned MCBitmap::GetHeight() const
{
    return MC_Y;
}


/******************************************************************************/
/**
 * Return the pixel factor in X-direction. 2 for MC.
 */
unsigned MCBitmap::GetPixelXFactor() const
{
    return 2;
}


/******************************************************************************/
/**
 * Return the color of a pixel. If the coordinates are out of range, return
 * black.
 */
const C64Color* MCBitmap::GetColor(unsigned x, unsigned y) const
{
    if ((x < GetWidth()) && (y < GetHeight()))
        return m_aMCBlock[y / MCBLOCK_HEIGHT][x / MCBLOCK_WIDTH].GetColor(x % MCBLOCK_WIDTH, y % MCBLOCK_HEIGHT);
    else
        return &black;
}


/*****************************************************************************/
void MCBitmap::SetBackground(C64Color col)
{
    int x;
    for (x = 0; x < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS; ++x)
        m_aMCBlock[0][x].SetMCColor(0, col);
}

/*****************************************************************************/
unsigned char MCBitmap::GetBackground() const
{
    return (unsigned char) m_aMCBlock[0][0].GetMCColor(0)->GetColor();
}

/*****************************************************************************/
void MCBitmap::SetScreenRAM(unsigned offset, unsigned char val)
{
    C64Color col;

    if (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS)
    {
        col.SetColor((val >> 4) & 0x0f);
        m_aMCBlock[0][offset].SetMCColor(1, col);

        col.SetColor(val & 0x0f);
        m_aMCBlock[0][offset].SetMCColor(2, col);
    }
}

/*****************************************************************************/
unsigned char MCBitmap::GetScreenRAM(unsigned offset) const
{
    unsigned char v;

    if (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS)
    {
        v  = m_aMCBlock[0][offset].GetMCColor(1)->GetColor() << 4;
        v |= m_aMCBlock[0][offset].GetMCColor(2)->GetColor();
    }
    return v;
}

/*****************************************************************************/
void MCBitmap::SetColorRAM(unsigned offset, unsigned char val)
{
    C64Color col;

    if (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS)
    {
        col.SetColor(val & 0x0f);
        m_aMCBlock[0][offset].SetMCColor(3, col);
    }
}

/*****************************************************************************/
unsigned char MCBitmap::GetColorRAM(unsigned offset) const
{
    unsigned char v;

    if (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS)
    {
        v = m_aMCBlock[0][offset].GetMCColor(3)->GetColor();
    }
    return v;
}

/*****************************************************************************/
void MCBitmap::SetBitmapRAM(unsigned offset, unsigned char val)
{
    if (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS * MCBITMAP_BYTES_PER_BLOCK)
        m_aMCBlock[0][offset / MCBITMAP_BYTES_PER_BLOCK].SetBitmapRAM(
            offset % MCBITMAP_BYTES_PER_BLOCK, val);
}

/*****************************************************************************/
unsigned char MCBitmap::GetBitmapRAM(unsigned offset)
{
    //ASSERT (offset < MCBITMAP_XBLOCKS * MCBITMAP_YBLOCKS * MCBITMAP_BYTES_PER_BLOCK);

    return m_aMCBlock[0][offset / MCBITMAP_BYTES_PER_BLOCK].GetBitmapRAM(
         offset % MCBITMAP_BYTES_PER_BLOCK);
}

/*****************************************************************************/
/*
 * Get a const reference to the block containing the given coordinates.
 * Return NULL if the coordinates are out of range
 */
const MCBlock* MCBitmap::GetMCBlock(unsigned x, unsigned y) const
{
    if ((x < GetWidth()) && (y < GetHeight()))
    {
        return &(m_aMCBlock[y / MCBLOCK_HEIGHT][x / MCBLOCK_WIDTH]);
    }
    else
        return NULL;
}

/*****************************************************************************
 * Setzt einen Pixel an x/y in der Farbe col. Gibt es schon 3 Vordergrund-
 * farben, wird je nach "mode" verfahren. Siehe MCBlock::SetPixel
 */
void MCBitmap::SetPixel(unsigned x, unsigned y,
                        const C64Color& col, MCDrawingMode mode)
{
    MCBlock* pBlock;

    if ((x < GetWidth()) && (y < GetHeight()))
    {
         pBlock = &(m_aMCBlock[y / MCBLOCK_HEIGHT][x / MCBLOCK_WIDTH]);
         pBlock->SetPixel(x % MCBLOCK_WIDTH, y % MCBLOCK_HEIGHT,
                          col, mode);
    }
}
