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

#include "BitmapBase.h"
#include "C64Color.h"


/*****************************************************************************/
/**
 * Constructor.
 */
BitmapBase::BitmapBase() :
    m_rectDirty(wxRect(-1, -1, 0, 0))
{
}


/*****************************************************************************/
/**
 * Return the width of a attribute cell, if applicable. Return (todo) if
 * this bitmap has no cells.
 */
int BitmapBase::GetCellWidth() const
{
    return 8;
}


/*****************************************************************************/
/**
 * Return the height of an attribute cell, if applicable. Return (todo) if
 * this bitmap has no cells.
 */
int BitmapBase::GetCellHeight() const
{
    return 8;
}


/*****************************************************************************/
/**
 * Return the pixel factor in X-direction. e.g. 1 for hires, 2 for MC.
 */
int BitmapBase::GetPixelXFactor() const
{
    return 1;
}


/*****************************************************************************/
/**
 * Return the pixel factor in Y-direction. 1 for most modes, 2 for Y-expanded
 * sprites.
 */
int BitmapBase::GetPixelYFactor() const
{
    return 1;
}


/*****************************************************************************/
/*
 * Sort and clip these coordinates so that x1/y1 <= x2/y2 and all of them are
 * clipped to the coordinate range of this document.
 */
void BitmapBase::SortAndClip(int* px1, int* py1, int* px2, int* py2)
{
    int tmp;

    if (*px1 < 0)
        *px1 = 0;
    else if (*px1 >= GetWidth())
        *px1 = GetWidth() - 1;

    if (*py1 < 0)
        *py1 = 0;
    else if (*py1 >= GetHeight())
        *py1 = GetHeight() - 1;

    if (*px2 < 0)
        *px2 = 0;
    else if (*px2 >= GetWidth())
        *px2 = GetWidth() - 1;

    if (*py2 < 0)
        *py2 = 0;
    else if (*py2 >= GetHeight())
        *py2 = GetHeight() - 1;

    if (*px1 > *px2)
    {
        tmp  = *px1;
        *px1 = *px2;
        *px2 = tmp; // swap
    }

    if (*py1 > *py2)
    {
        tmp  = *py1;
        *py1 = *py2;
        *py2 = tmp; // swap
    }
}


/*****************************************************************************/
/**
 * Reset the dirty area to size (0, 0).
 */
void BitmapBase::ResetDirty()
{
    m_rectDirty = wxRect(-1, -1, 0, 0);
}

/*****************************************************************************/
/**
 * Extend the dirty area to contain x/y.
 */
void BitmapBase::Dirty(int x, int y)
{
    if (m_rectDirty.GetRight() < 0)
    {
        m_rectDirty.SetX(x);
        m_rectDirty.SetY(y);
        m_rectDirty.SetWidth(1);
        m_rectDirty.SetHeight(1);
    }
    else
    {
        m_rectDirty.Union(wxRect(x, y, 1, 1));
    }
}


/*****************************************************************************/
/**
 * Extend the dirty area to contain x/y/w/h.
 */
void BitmapBase::Dirty(int x, int y, int w, int h)
{
    if (m_rectDirty.GetRight() < 0)
    {
        m_rectDirty.SetX(x);
        m_rectDirty.SetY(y);
        m_rectDirty.SetWidth(w);
        m_rectDirty.SetHeight(h);
    }
    else
    {
        m_rectDirty.Union(wxRect(x, y, w, h));
    }
}


/*****************************************************************************/
/**
 * Fill pixel x/y and the adjacent pixels with the same color as this one
 * with color col. If a color limit is hit, do what the drawing mode requires.
 */
void BitmapBase::FloodFill(unsigned x, unsigned y,
                           const C64Color& col, MCDrawingMode mode)
{
    typedef enum { FF_UNCHECKED = 0, FF_TOCHECK, FF_CHECKED } state_t;
    C64Color colOld;
    bool     bGoOn;

    unsigned w = GetWidth();
    unsigned h = GetHeight();

    state_t  state[h][w];

    memset(&state, FF_UNCHECKED, sizeof(state));
    colOld = *GetColor(x, y);
    state[y][x] = FF_TOCHECK;

    do
    {
        bGoOn = false;
        for (y = 0; y < h; ++y)
        {
            for (x = 0; x < w; ++x)
            {
                if (state[y][x] == FF_TOCHECK)
                {
                    if (x > 0 && state[y][x - 1] == FF_UNCHECKED &&
                        *GetColor(x - 1, y) == colOld)
                        state[y][x - 1] = FF_TOCHECK;
                    if (x + 1 < w && state[y][x + 1] == FF_UNCHECKED &&
                        *GetColor(x + 1, y) == colOld)
                        state[y][x + 1] = FF_TOCHECK;
                    if (y > 0 && state[y - 1][x] == FF_UNCHECKED &&
                        *GetColor(x, y - 1) == colOld)
                        state[y - 1][x] = FF_TOCHECK;
                    if (y + 1 < h && state[y + 1][x] == FF_UNCHECKED &&
                        *GetColor(x, y + 1) == colOld)
                        state[y + 1][x] = FF_TOCHECK;
                    state[y][x] = FF_CHECKED;
                    SetPixel(x, y, col, mode);
                    bGoOn = true;
                }
            }
        }
    }
    while (bGoOn);
}

/*****************************************************************************/
/**
 * Draw a line with color col.
 * If a color limit is hit, do what the drawing mode requires.
 */
void BitmapBase::Line(int x1, int y1, int x2, int y2,
                      const C64Color& col, MCDrawingMode mode)
{
    int fixx, fixy, step;

    if (abs(x2 - x1) > abs(y2 - y1))
    {
        /* horizontal */
        if (x2 < x1)
        {
            step = x1; x1 = x2; x2 = step;
            step = y1; y1 = y2; y2 = step;
        }

        fixy = y1 * 1024;
        step = x2 == x1 ? 0 : 1024 * (y2 - y1) / (x2 - x1);
        for (fixx = x1 * 1024; fixx <= x2 * 1024; fixx += 1024)
        {
            SetPixel((fixx + 512) / 1024, (fixy + 512) / 1024,
                     col, mode);
            fixy += step;
        }
    }
    else
    {
        /* vertical */
        if (y2 < y1)
        {
            step = x1; x1 = x2; x2 = step;
            step = y1; y1 = y2; y2 = step;
        }

        fixx = x1 * 1024;
        step = y2 == y1 ? 0 : 1024 * (x2 - x1) / (y2 - y1);
        for (fixy = y1 * 1024; fixy <= y2 * 1024; fixy += 1024)
        {
            SetPixel((fixx + 512) / 1024, (fixy + 512) / 1024,
                     col, mode);
            fixx += step;
        }
    }
}
