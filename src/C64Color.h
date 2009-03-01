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

#ifndef C64COLOR_H
#define C64COLOR_H

#include <wx/colour.h>

typedef unsigned long MC_RGB;

#define MC_RGB_COLOR(r,g,b) (((r) << 16) | ((g) << 8) | (b))
#define MC_RGB_R(rgb) (((rgb) >> 16) & 0xff)
#define MC_RGB_G(rgb) (((rgb) >> 8) & 0xff)
#define MC_RGB_B(rgb) ((rgb) & 0xff)

#define MC_BLACK   0
#define MC_WHITE   1
#define MC_RED     2
#define MC_CYAN    3
#define MC_PURPLE  4
#define MC_GREEN   5
#define MC_BLUE    6
#define MC_YELLOW  7
#define MC_ORANGE  8
#define MC_BROWN   9
#define MC_LRED   10
#define MC_DGRAY  11
#define MC_GRAY   12
#define MC_LGREEN 13
#define MC_LBLUE  14
#define MC_LGRAY  15

#define MC_GRID_COL_R 0x22
#define MC_GRID_COL_G 0x11
#define MC_GRID_COL_B 0x11

class C64Color
{
protected:
    int m_color;
    MC_RGB m_RGB;

public:
    C64Color(void);
    C64Color(int);

    void SetColor(int col);
    int GetColor(void) const;

    inline MC_RGB GetRGB(void) const
    {
        return m_RGB;
    }

    // avoid using this, that's slow
    inline wxColour GetWxColor(void) const
    {
        return wxColour((m_RGB >> 16) & 0xff, (m_RGB >> 8) & 0xff, m_RGB & 0xff);
    }

    MC_RGB GetContrastRGB(void) const;

    inline bool operator==(C64Color c2) const
    {
        return (m_color == c2.m_color);
    }
};

#endif
