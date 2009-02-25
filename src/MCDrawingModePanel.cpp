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

#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/msgdlg.h>

#include "MCDrawingModePanel.h"

MCDrawingModePanel::MCDrawingModePanel(wxWindow* pParent):
    wxPanel(pParent)
{
    wxStaticBoxSizer* pSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Drawing Mode"));

    m_pButtonReplaceLeastUsed =
        new wxRadioButton(this, wxID_ANY, wxT("Replace least used"));
    m_pButtonReplaceCurrent =
        new wxRadioButton(this, wxID_ANY, wxT("Replace current"));
    m_pButtonIgnore =
        new wxRadioButton(this, wxID_ANY, wxT("Ignore 4th color"));

    pSizer->Add(m_pButtonReplaceLeastUsed);
    pSizer->Add(m_pButtonReplaceCurrent);
    pSizer->Add(m_pButtonIgnore);

    SetSizer(pSizer);
    pSizer->Fit(this);
    SetMinSize(GetSize());

    // Default: "Least used"
    m_pButtonReplaceLeastUsed->SetValue(true);
    m_drawingMode = MCDrawingModeLeast;

    Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(MCDrawingModePanel::OnRadioButtonSelected));
}

MCDrawingModePanel::~MCDrawingModePanel()
{
}


/*****************************************************************************/
/*
 * Is called when a radio button is selected. Remember the selected
 * drawing mode.
 */
void MCDrawingModePanel::OnRadioButtonSelected(wxCommandEvent& event)
{
    if (event.GetEventObject() == m_pButtonReplaceLeastUsed)
    {
        m_drawingMode = MCDrawingModeLeast;
    }
    else if (event.GetEventObject() == m_pButtonReplaceCurrent)
    {
        m_drawingMode = MCDrawingModeForce;
    }
    else if (event.GetEventObject() == m_pButtonIgnore)
    {
        m_drawingMode = MCDrawingModeIgnore;
    }
}
