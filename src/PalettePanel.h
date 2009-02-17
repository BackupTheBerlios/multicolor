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

#ifndef PALETTEPANEL_H
#define PALETTEPANEL_H

#include <wx/panel.h>

class PalettePanel: public wxPanel
{
public:
    PalettePanel(wxWindow* parent);
    int GetColorA();
    int GetColorB();

private:
    wxPanel* m_pPanelSelA;
    wxPanel* m_pPanelSelB;
    wxPanel* m_apPanelColor[16];

    // selected colors 0..15
    int m_nColorA;
    int m_nColorB;

    void OnColorLeftDown(wxMouseEvent& event);
    void OnColorRightDown(wxMouseEvent& event);
};

/*****************************************************************************/
inline int PalettePanel::GetColorA()
{
    return m_nColorA;
}

/*****************************************************************************/
inline int PalettePanel::GetColorB()
{
    return m_nColorB;
}

#endif
