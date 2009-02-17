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

#include <wx/app.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>

#include "MCApp.h"
#include "C64Color.h"
#include "PalettePanel.h"

/*****************************************************************************/
/*
 * Create a palette panel.
 * This is constructed like this:
 * 
 * PalettePanel::wxPanel:
 * -----------------------------------------------------------
 * |pBoxSizerOuter                                           |
 * | ------------------------------------------------------- |
 * | |pGridSizerInner                                      | |
 * | |                                                     | |
 * | | Has 4x4 wxPanels, one for each color                | |
 * | | (m_apPanelColor)                                    | |
 * | |                                                     | |
 * | ------------------------------------------------------- |
 * |                                                         |
 * | ------------------------------------------------------- |
 * | |pBoxSizerSelections                                  | |
 * | |    -----------------  -------  -----------------    | |
 * | |    | m_pPanelSelA  |  |Mouse|  | m_pPanelSelB  |    | |
 * | |    |               |  |     |  |               |    | |
 * | |    -----------------  -------  -----------------    | |
 * | ------------------------------------------------------- |
 * |                                                         |
 * -----------------------------------------------------------
 *
 */ 
PalettePanel::PalettePanel(wxWindow* parent)
    : wxPanel(parent)
    , m_nColorA(1)
    , m_nColorB(0)
{
    int i;
    C64Color      c64Color;
    wxBoxSizer*   pBoxSizerOuter;
    wxGridSizer*  pGridSizerInner;
    wxBoxSizer*   pBoxSizerSelections;
    wxStaticBitmap* pBitmapMouse;

	pBoxSizerOuter = new wxBoxSizer(wxVERTICAL);

	// prepare the grid sizer and add it into the outer sizer
	pGridSizerInner = new wxGridSizer(0, 4, 0, 0);
    pBoxSizerOuter->Add(pGridSizerInner, 0, wxFIXED_MINSIZE | wxALIGN_LEFT | wxALIGN_TOP, 0);

    // add a panel for each color into the grid sizer
    for (i = 0; i < 16; i++)
    {
        c64Color.SetColor(i);

        m_apPanelColor[i] = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(18, 18), wxRAISED_BORDER|wxTAB_TRAVERSAL);
        pGridSizerInner->Add(m_apPanelColor[i], 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);

        m_apPanelColor[i]->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(PalettePanel::OnColorLeftDown), NULL, this);
        m_apPanelColor[i]->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(PalettePanel::OnColorRightDown), NULL, this);
        m_apPanelColor[i]->SetBackgroundColour(c64Color.GetWxColor());
    }

    // Add the two panels for the current color
    pBoxSizerSelections = new wxBoxSizer(wxHORIZONTAL);

    m_pPanelSelA = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(20,16), 
            wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
    c64Color.SetColor(m_nColorA);
    m_pPanelSelA->SetBackgroundColour(c64Color.GetWxColor());
    pBoxSizerSelections->Add(m_pPanelSelA);

    // Add a label with a mouse-icon
    pBitmapMouse = new wxStaticBitmap(this, wxID_ANY, wxGetApp().GetBitmap(wxT("24x24"), wxT("mouse.png")));
    pBoxSizerSelections->Add(pBitmapMouse);

    m_pPanelSelB = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(20,16),
            wxSIMPLE_BORDER | wxTAB_TRAVERSAL);
    c64Color.SetColor(m_nColorB);
    
    m_pPanelSelB->SetBackgroundColour(c64Color.GetWxColor());
    pBoxSizerSelections->Add(m_pPanelSelB);

    // Add the sizer for the current colors into the outer sizer 
    pBoxSizerOuter->Add(pBoxSizerSelections, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(pBoxSizerOuter);

    pBoxSizerOuter->Fit(this);
    SetMinSize(GetSize());
}


void PalettePanel::OnColorLeftDown(wxMouseEvent& event)
{
    int i;
    C64Color c64Color;

    for (i = 0; i < 16; i++)
    {
        if (m_apPanelColor[i]->GetId() == event.GetId())
        {
            m_nColorA = i;
            c64Color.SetColor(m_nColorA);
            m_pPanelSelA->SetBackgroundColour(c64Color.GetWxColor());
            break;
        }
    }
}

void PalettePanel::OnColorRightDown(wxMouseEvent& event)
{
    int i;
    C64Color c64Color;

    for (i = 0; i < 16; i++)
    {
        if (m_apPanelColor[i]->GetId() == event.GetId())
        {
            m_nColorB = i;
            c64Color.SetColor(m_nColorB);
            m_pPanelSelB->SetBackgroundColour(c64Color.GetWxColor());
            break;
        }
    }
}

