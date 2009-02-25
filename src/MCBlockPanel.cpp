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

#include <wx/sizer.h>

#include "MCBlock.h"
#include "MCBlockPanel.h"
#include "C64Color.h"
#include "MCDoc.h"

/*****************************************************************************/
/*
 * Create a palette panel.
 * This is constructed like this:
 *
 * PalettePanel::wxPanel:
 * -----------------------------------------------------------------------
 * |pSizerOuter                                                          |
 * |                     ----------------------------------------------- |
 * |                     |pRightGridSizer                              | |
 * |-------------------  | --------------------- --------------------- | |
 * ||pGridSizer       |  | |m_apUsagePanel[0]  | |m_apUsageStaticText| | |
 * ||                 |  | --------------------- --------------------- | |
 * ||                 |  | --------------------- --------------------- | |
 * ||                 |  | |m_apUsagePanel[1]  | |m_apUsageStaticText| | |
 * ||    4 x 8        |  | --------------------- --------------------- | |
 * ||  m_apPixelPanel |  | --------------------- --------------------- | |
 * ||                 |  | |m_apUsagePanel[2]  | |m_apUsageStaticText| | |
 * ||                 |  | --------------------- --------------------- | |
 * ||                 |  | --------------------- --------------------- | |
 * ||                 |  | |m_apUsagePanel[3]  | |m_apUsageStaticText| | |
 * |-------------------  | --------------------- --------------------- | |
 * |                     ----------------------------------------------- |
 * -----------------------------------------------------------------------
 */
MCBlockPanel::MCBlockPanel(wxWindow* pParent) :
    wxPanel(pParent)
{
    int x, y;
    wxPanel*     pPanel;
    wxStaticText* pText;
    wxStaticBoxSizer* pSizerOuter = new wxStaticBoxSizer(wxHORIZONTAL, this, wxT("Current Cell"));
    wxFlexGridSizer* pRightGridSizer = new wxFlexGridSizer(3);
    wxGridSizer* pGridSizer = new wxGridSizer(MCBLOCK_WIDTH);

    pSizerOuter->AddStretchSpacer();
    pSizerOuter->Add(pGridSizer);
    pSizerOuter->Add(pRightGridSizer);

    for (y = 0; y < MCBLOCK_HEIGHT; ++y)
    {
        for (x = 0; x < MCBLOCK_WIDTH; ++x)
        {
            pPanel = new wxPanel(this, wxID_ANY,
                    wxPoint(2 * m_nPixelHeight * x, m_nPixelHeight * y),
                    wxSize(2 * m_nPixelHeight, m_nPixelHeight),
                    wxBORDER_SIMPLE);
            pGridSizer->Add(pPanel);
            pPanel->SetBackgroundColour(wxColor(0, 0, 0));
            m_apPixelPanel[y][x] = pPanel;
        }
    }

    for (y = 0; y < 4; ++y)
    {
        pText = new wxStaticText(this, wxID_ANY, wxString::Format(wxT("C%d"), y));
        pRightGridSizer->Add(pText, 0, wxLEFT | wxRIGHT, 10);

        pPanel = new wxPanel(this, wxID_ANY,
                wxPoint(2 * m_nPixelHeight * x, m_nPixelHeight * y),
                wxSize(2 * m_nPixelHeight, pText->GetSize().GetHeight()),
                wxBORDER_SIMPLE);
        pRightGridSizer->Add(pPanel);
        m_apUsagePanel[y] = pPanel;

        pText = new wxStaticText(this, wxID_ANY, wxT("0 x"));
        pRightGridSizer->Add(pText, 0, wxLEFT, 10);
        m_apUsageStaticText[y] = pText;
    }

    pSizerOuter->AddStretchSpacer();

    SetSizer(pSizerOuter);
    pSizerOuter->Fit(this);
    SetMinSize(GetSize());
}

MCBlockPanel::~MCBlockPanel()
{
}

/*****************************************************************************/
/*
 * Show the block which contains the given bitmap coordinates.
 */
void MCBlockPanel::ShowBlock(MCDoc* pDoc, unsigned x, unsigned y)
{
    unsigned  xx, yy;
    const C64Color* pC64Color;
    wxPanel*  pPanel;

    const MCBlock* pBlock = pDoc->m_bitmap.GetMCBlock(x, y);
    if (!pBlock)
        return;

    for (yy = 0; yy < MCBLOCK_HEIGHT; ++yy)
    {
        for (xx = 0; xx < MCBLOCK_WIDTH; ++xx)
        {
            pC64Color = pBlock->GetColor(xx, yy);

            pPanel = m_apPixelPanel[yy][xx];
            pPanel->SetBackgroundColour(pC64Color->GetWxColor());
            pPanel->Refresh();
        }
    }

    // count the 4 colors
    for (yy = 0; yy < 4; ++yy)
    {
        m_apUsagePanel[yy]->SetBackgroundColour(pBlock->GetMCColor(yy)->GetWxColor());
        m_apUsagePanel[yy]->Refresh();
        m_apUsageStaticText[yy]->SetLabel(wxString::Format(wxT("%d x"), pBlock->CountMCIndex(yy)));
    }
}
