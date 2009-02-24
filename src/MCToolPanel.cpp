/*
 * MCToolPanel.cpp
 *
 *  Created on: 24.02.2009
 *      Author: skoe
 */

#include <wx/sizer.h>

#include "MCToolPanel.h"
#include "PalettePanel.h"
#include "MCCanvas.h"

MCToolPanel::MCToolPanel(wxWindow* parent):
    wxPanel(parent)
{
    wxBoxSizer*   pBoxSizerOuter;

    pBoxSizerOuter = new wxBoxSizer(wxVERTICAL);

    m_pCanvas = new MCCanvas(NULL, this, wxBORDER_SUNKEN);
    pBoxSizerOuter->Add(m_pCanvas);

    pBoxSizerOuter->AddSpacer(4);

    m_pPalettePanel = new PalettePanel(this);
    pBoxSizerOuter->Add(m_pPalettePanel, wxSizerFlags().Center());

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
void MCToolPanel::SetActiveChild(MCChildFrame* pFrame)
{
    m_pCanvas->SetChildFrame(pFrame);
}

/******************************************************************************/
/*
 * Refresh the preview because the image may have changed.
 */
void MCToolPanel::Refresh()
{
    m_pCanvas->Refresh();
}
