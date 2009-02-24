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

#ifndef MCTOOLPANEL_H_
#define MCTOOLPANEL_H_

#include <wx/panel.h>

class PalettePanel;
class MCChildFrame;
class MCCanvas;

class MCToolPanel : public wxPanel
{
public:
    MCToolPanel(wxWindow* parent);
    virtual ~MCToolPanel();

    void SetActiveChild(MCChildFrame* pFrame);
    void Refresh();
    PalettePanel* GetPalettePanel();

protected:
    MCCanvas*     m_pCanvas;
    PalettePanel* m_pPalettePanel;
};

/*****************************************************************************/
inline PalettePanel* MCToolPanel::GetPalettePanel()
{
    return m_pPalettePanel;
}

#endif /* MCTOOLPANEL_H_ */
