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
#include <wx/msgdlg.h>

#include "MCToolPanel.h"
#include "MCCanvas.h"
#include "PalettePanel.h"
#include "MCDrawingModePanel.h"
#include "MCBlockPanel.h"
#include "MCChildFrame.h"


MCToolPanel::MCToolPanel(wxWindow* parent):
    wxPanel(parent)
{
    wxBoxSizer*   pBoxSizerOuter;
    wxGridSizer*  pGridSizerColors;

    pBoxSizerOuter = new wxBoxSizer(wxVERTICAL);

    m_pCanvas = new MCCanvas(NULL, this, wxBORDER_SUNKEN);
    pBoxSizerOuter->Add(m_pCanvas);
	wxMessageBox(wxString::Format(wxT("canvas %p"), m_pCanvas));

    pBoxSizerOuter->AddSpacer(4);
    pGridSizerColors = new wxGridSizer(2, 1, 10);
    pBoxSizerOuter->Add(pGridSizerColors);

    m_pPalettePanel = new PalettePanel(this);
    pGridSizerColors->Add(m_pPalettePanel, 1, wxALIGN_CENTER_HORIZONTAL);

    m_pDrawingModePanel = new MCDrawingModePanel(this);
    pGridSizerColors->Add(m_pDrawingModePanel, 1, wxALIGN_CENTER_HORIZONTAL);

    m_pBlockPanel = new MCBlockPanel(this);
    pBoxSizerOuter->Add(m_pBlockPanel, wxSizerFlags().Expand());

    SetSizer(pBoxSizerOuter);
    pBoxSizerOuter->Fit(this);
    SetMinSize(GetSize());
}

MCToolPanel::~MCToolPanel()
{
    // TODO Auto-generated destructor stub
}

/*****************************************************************************/
/*
 * Set the active child window which shall be used to update the preview
 * and so on from now.
 */
void MCToolPanel::SetActiveView(MCChildFrame* pFrame)
{
    m_pActiveView = pFrame;
    m_pCanvas->SetChildFrame(pFrame);
}

/*****************************************************************************/
/*
 * Set the current mouse position which shall be used to update the preview
 * and so on from now.
 */
void MCToolPanel::SetMousePos(int x, int y)
{
    m_pCanvas->SetMousePos(x, y);
    m_pBlockPanel->ShowBlock(m_pActiveView->GetDocument(), x, y);
}

/******************************************************************************/
/*
 * Refresh the preview because the image may have changed.
 */
void MCToolPanel::Refresh()
{
    m_pCanvas->Refresh();
}
