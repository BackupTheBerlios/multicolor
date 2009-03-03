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

#include "MCToolPanel.h"
#include "MCCanvas.h"
#include "PalettePanel.h"
#include "MCDrawingModePanel.h"
#include "MCBlockPanel.h"


MCToolPanel::MCToolPanel(wxWindow* parent):
    wxPanel(parent)
{
    wxBoxSizer*       pBoxSizerOuter;
    wxGridSizer*      pGridSizerColors;
    wxStaticBoxSizer* pStaticBoxSizerBlock;

    pBoxSizerOuter = new wxBoxSizer(wxVERTICAL);

    m_pCanvas = new MCCanvas(this, wxBORDER_SUNKEN);
    pBoxSizerOuter->Add(m_pCanvas);

    pBoxSizerOuter->AddSpacer(4);
    pGridSizerColors = new wxGridSizer(2, 1, 10);
    pBoxSizerOuter->Add(pGridSizerColors);

    m_pPalettePanel = new PalettePanel(this);
    pGridSizerColors->Add(m_pPalettePanel, 1, wxALIGN_CENTER_HORIZONTAL);

    m_pDrawingModePanel = new MCDrawingModePanel(this);
    pGridSizerColors->Add(m_pDrawingModePanel, 1, wxALIGN_CENTER_HORIZONTAL);

    pStaticBoxSizerBlock = new
        wxStaticBoxSizer(wxVERTICAL, this, wxT("Current Cell"));
    pBoxSizerOuter->Add(pStaticBoxSizerBlock, wxSizerFlags().Expand());

    m_pBlockPanel = new MCBlockPanel(this);
    pStaticBoxSizerBlock->Add(m_pBlockPanel, wxSizerFlags().Center());

    SetSizer(pBoxSizerOuter);
    pBoxSizerOuter->Fit(this);
    SetMinSize(GetSize());
}


/*****************************************************************************/
MCToolPanel::~MCToolPanel()
{
}


/*****************************************************************************/
/*
 * Set the active document which shall be used to update the preview
 * and so on from now. NULL means nothing to draw.
 */
void MCToolPanel::SetActiveDoc(MCDoc* pDoc)
{
    if (pDoc)
    {
        m_pCanvas->SetDoc(pDoc);
        m_pBlockPanel->SetDoc(pDoc);
    }
    else
    {
        m_pCanvas->SetDoc(NULL);
        m_pBlockPanel->SetDoc(NULL);
    }
}


/******************************************************************************/
/*
 * Refresh the preview because the image may have changed.
 */
void MCToolPanel::Refresh()
{
    m_pCanvas->Refresh();
}
