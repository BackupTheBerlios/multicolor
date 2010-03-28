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

#include <stdlib.h>
#include <string.h>
#include "HiResBitmap.h"
#include "ToolBase.h"


const C64Color HiResBitmap::black;


/*****************************************************************************/
HiResBitmap::HiResBitmap(void)
{
    int x;
    for (x = 0; x < HIRESBITMAP_XBLOCKS * HIRESBITMAP_YBLOCKS; ++x)
        m_aHiResBlock[0][x].SetParent(this);
}

/*****************************************************************************/
HiResBitmap::~HiResBitmap(void)
{
}


/******************************************************************************/
/**
 * Return a pointer to a copy of this bitmap created with "new".
 */
BitmapBase* HiResBitmap::Copy() const
{
    return new HiResBitmap(*this);
}


/******************************************************************************/
/**
 * Return the width of this image.
 */
int HiResBitmap::GetWidth() const
{
    return HIRES_X;
}


/******************************************************************************/
/**
 * Return the height of this image.
 */
int HiResBitmap::GetHeight() const
{
    return HIRES_Y;
}


/*****************************************************************************/
/**
 * Return the number of color indexes in this mode.
 */
int HiResBitmap::GetNIndexes() const
{
    return 2;
}


/******************************************************************************/
/**
 * Return the color for of an index which is set for the macro cell which
 * contains the given coordinates. If the coordinates are out of range,
 * return black.
 */
const C64Color* HiResBitmap::GetColorByIndex(int x, int y, int index) const
{
    if ((x >= 0) && (y >= 0) && (x < GetWidth()) && (y < GetHeight()))
    {
        x /= HIRESBLOCK_WIDTH;
        y /= HIRESBLOCK_HEIGHT;
        return m_aHiResBlock[y][x].GetIndexedColor(index);
    }
    else
        return &black;
}


/******************************************************************************/
/**
 * Return the number of pixels of an index for the macro cell which contains
 * the given coordinates. If the coordinates are out of range, return 0.
 */
int HiResBitmap::CountColorByIndex(int x, int y, int index) const
{
    if ((x >= 0) && (y >= 0) && (x < GetWidth()) && (y < GetHeight()))
    {
        x /= HIRESBLOCK_WIDTH;
        y /= HIRESBLOCK_HEIGHT;
        return m_aHiResBlock[y][x].CountIndexedColor(index);
    }
    else
        return 0;
}


/******************************************************************************/
/**
 * Return the color of a pixel. If the coordinates are out of range, return
 * black.
 */
const C64Color* HiResBitmap::GetColor(int x, int y) const
{
    if ((x >= 0) && (y >= 0) && (x < GetWidth()) && (y < GetHeight()))
    {
        return m_aHiResBlock[y / HIRESBLOCK_HEIGHT][x / HIRESBLOCK_WIDTH].
                GetPixel(x % HIRESBLOCK_WIDTH, y % HIRESBLOCK_HEIGHT);
    }
    else
        return &black;
}


/*****************************************************************************/
void HiResBitmap::SetScreenRAM(unsigned offset, unsigned char val)
{
    C64Color col;

    col.SetColor((val >> 4) & 0x0f);
    m_aHiResBlock[0][offset].SetIndexedColor(0, col);

    col.SetColor(val & 0x0f);
    m_aHiResBlock[0][offset].SetIndexedColor(1, col);
}


/*****************************************************************************/
unsigned char HiResBitmap::GetScreenRAM(unsigned offset) const
{
    unsigned char v;

    v  = m_aHiResBlock[0][offset].GetIndexedColor(0)->GetColor() << 4;
    v |= m_aHiResBlock[0][offset].GetIndexedColor(1)->GetColor();
    return v;
}


/*****************************************************************************/
void HiResBitmap::SetBitmapRAM(unsigned offset, unsigned char val)
{
    m_aHiResBlock[0][offset / HIRESBITMAP_BYTES_PER_BLOCK].SetBitmapRAM(
        offset % HIRESBITMAP_BYTES_PER_BLOCK, val);
}


/*****************************************************************************/
unsigned char HiResBitmap::GetBitmapRAM(unsigned offset)
{
    return m_aHiResBlock[0][offset / HIRESBITMAP_BYTES_PER_BLOCK].GetBitmapRAM(
         offset % HIRESBITMAP_BYTES_PER_BLOCK);
}

/*****************************************************************************/
/**
 * Get a the block containing the given coordinates.
 * Return NULL if the coordinates are out of range
 */
const HiResBlock* HiResBitmap::GetHiResBlock(unsigned x, unsigned y) const
{
    if ((x < GetWidth()) && (y < GetHeight()))
    {
        return &(m_aHiResBlock[y / HIRESBLOCK_HEIGHT][x / HIRESBLOCK_WIDTH]);
    }
    else
        return NULL;
}

/*****************************************************************************
 * Setzt einen Pixel an x/y in der Farbe col. Gibt es schon alle Vordergrund-
 * farben, wird je nach "mode" verfahren. Siehe HiResBlock::SetPixel
 */
void HiResBitmap::SetPixel(int x, int y,
                        const C64Color& col, MCDrawingMode mode)
{
    HiResBlock* pBlock;

    if ((x >= 0) && (y >= 0) && (x < GetWidth()) && (y < GetHeight()))
    {
        pBlock = &(m_aHiResBlock[y / HIRESBLOCK_HEIGHT][x / HIRESBLOCK_WIDTH]);
        pBlock->SetPixel(x % HIRESBLOCK_WIDTH, y % HIRESBLOCK_HEIGHT,
                         col, mode);

        // this may change the whole block
        Dirty(x & ~(HIRESBLOCK_WIDTH - 1), y & ~(HIRESBLOCK_HEIGHT - 1),
              HIRESBLOCK_WIDTH, HIRESBLOCK_HEIGHT);
    }
}
