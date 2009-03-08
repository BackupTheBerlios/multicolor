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
#include "C64Color.h"

static const MC_RGB m_aRGB[16] = {
    MC_RGB_COLOR(0, 0, 0),          MC_RGB_COLOR(0xff, 0xff, 0xff),
    MC_RGB_COLOR(0x89, 0x40, 0x36), MC_RGB_COLOR(0x7a, 0xbf, 0xc7),
    MC_RGB_COLOR(0x8a, 0x46, 0xae), MC_RGB_COLOR(0x68, 0xa9, 0x41),
    MC_RGB_COLOR(0x3e, 0x31, 0xa2), MC_RGB_COLOR(0xd0, 0xdc, 0x71),
    MC_RGB_COLOR(0x90, 0x5f, 0x25), MC_RGB_COLOR(0x5c, 0x47, 0x00),
    MC_RGB_COLOR(0xbb, 0x77, 0x6d), MC_RGB_COLOR(0x55, 0x55, 0x55),
    MC_RGB_COLOR(0x80, 0x80, 0x80), MC_RGB_COLOR(0xac, 0xea, 0x88),
    MC_RGB_COLOR(0x7c, 0x70, 0xda), MC_RGB_COLOR(0xab, 0xab, 0xab) };

/*****************************************************************************/
C64Color::C64Color(void)
{
    m_color = MC_BLACK;
    m_RGB   = m_aRGB[m_color];
}

/*****************************************************************************/
C64Color::C64Color(int col)
{
    m_color = col;
    m_RGB   = m_aRGB[m_color];
}

/*****************************************************************************/
void C64Color::SetColor(int col)
{
    m_color = col;
    m_RGB   = m_aRGB[m_color];
}

/*****************************************************************************/
int C64Color::GetColor(void) const
{
    return m_color;
}

/*****************************************************************************/
MC_RGB C64Color::GetContrastRGB(void) const
{
    switch (m_color)
    {
    case MC_RED:
    case MC_ORANGE:
    case MC_PURPLE:
    case MC_LRED:
        return MC_RGB_COLOR(64, 64, 255);

    default:
        return MC_RGB_COLOR(255, 64, 64);
    }
}



