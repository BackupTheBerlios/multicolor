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

#ifndef MCDRAWINGMODEPANEL_H_
#define MCDRAWINGMODEPANEL_H_

#include <wx/panel.h>
#include "MCToolBase.h"

class wxRadioButton;

class MCDrawingModePanel : public wxPanel
{
public:
    MCDrawingModePanel(wxWindow* pParent);
    virtual ~MCDrawingModePanel();
    MCDrawingMode GetDrawingMode();

    void OnRadioButtonSelected(wxCommandEvent& event);

protected:
    wxRadioButton* m_pButtonIgnore;
    wxRadioButton* m_pButtonReplaceCurrent;
    wxRadioButton* m_pButtonReplaceLeastUsed;
    wxRadioButton* m_pButtonIndex0;
    wxRadioButton* m_pButtonIndex1;
    wxRadioButton* m_pButtonIndex2;
    wxRadioButton* m_pButtonIndex3;

    MCDrawingMode  m_drawingMode;
};


/*****************************************************************************/
inline MCDrawingMode MCDrawingModePanel::GetDrawingMode()
{
    return m_drawingMode;
}

#endif /* MCDRAWINGMODEPANEL_H_ */
