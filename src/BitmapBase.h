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

#ifndef BITMAPBASE_H
#define BITMAPBASE_H

#include <wx/gdicmn.h>

#include "MCToolBase.h"

class C64Color;

class BitmapBase
{
public:
    BitmapBase();

    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;

    virtual int GetPixelXFactor() const;
    virtual int GetPixelYFactor() const;

    void SortAndClip(int* px1, int* py1, int* px2, int* py2);
    void ResetDirty();
    void Dirty(int x, int y);
    const wxRect& GetDirtyRect() const;

    /**
     * Return the color of a pixel. If the coordinates are out of range, return
     * black.
     */
    virtual const C64Color* GetColor(int x, int y) const = 0;

    virtual void SetPixel(int x, int y, const C64Color& col,
                          MCDrawingMode mode = MCDrawingModeIgnore) = 0;

    virtual void FloodFill(unsigned x, unsigned y, const C64Color& col,
                           MCDrawingMode mode = MCDrawingModeIgnore);

    virtual void Line(int x1, int y1, int x2, int y2,
                      const C64Color& col, MCDrawingMode mode);

protected:
    wxRect m_rectDirty;
};


inline const wxRect& BitmapBase::GetDirtyRect() const
{
    return m_rectDirty;
}

#endif // BITMAPBASE_H
