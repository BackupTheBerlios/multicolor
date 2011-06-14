/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) 2003-2010 Thomas Giesel
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
#include <wx/stattext.h>
#include <wx/listctrl.h>

#include "NewFileDialog.h"
#include "FormatInfo.h"

/*
 * -pOuterSizer---------------------------------------------------
 * |                                                             |
 * | -pMainSizer------------------------------------------------ |
 * | |                            |                            | |
 * | ----------------------------------------------------------- |
 * | |                            |                            | |
 * | ----------------------------------------------------------- |
 * | |                            |                            | |
 * | ----------------------------------------------------------- |
 * | |                            |                            | |
 * | ----------------------------------------------------------- |
 * |                         pButtonSizer                        |
 * ---------------------------------------------------------------
 */

/*****************************************************************************/
/**
 */
NewFileDialog::NewFileDialog(wxWindow* parent) :
    wxDialog(parent, wxID_ANY, wxT("New file"), wxDefaultPosition, wxDefaultSize),
    m_pSelectedFormatInfo(NULL)
{
    wxStaticText*   pText;
    wxBoxSizer*     pOuterSizer;
    wxSizer*        pButtonSizer;
    std::list<const FormatInfo*>::const_iterator i;
    long         nItem;

    pOuterSizer = new wxBoxSizer(wxVERTICAL);
    pOuterSizer->AddSpacer(10);

    pText = new wxStaticText(this, wxID_ANY, wxT("Image format:"));
    pOuterSizer->Add(pText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);

    m_pTypeListCtrl = new wxListCtrl(this, wxID_ANY,
        wxDefaultPosition, wxSize(300, 120), wxLC_SINGLE_SEL | wxBORDER_SUNKEN);

    // Fill the list control with all supported formats
    for (i = FormatInfo::GetFormatList()->begin();
         i != FormatInfo::GetFormatList()->end();
         ++i)
    {
        nItem = m_pTypeListCtrl->InsertItem(0, (*i)->GetName());
        m_pTypeListCtrl->SetItemPtrData(nItem, (wxUIntPtr) *i);
    }
    m_pTypeListCtrl->SetItemState(0, wxLIST_STATE_SELECTED,
                                  wxLIST_STATE_SELECTED);

    pOuterSizer->Add(m_pTypeListCtrl);
    pOuterSizer->AddSpacer(10);

    pButtonSizer = CreateButtonSizer(wxOK | wxCANCEL);
    pOuterSizer->Add(pButtonSizer, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

    SetSizerAndFit(pOuterSizer);

    ConnectEventHandlers();
}


/*****************************************************************************/
/**
 * dtor
 */
NewFileDialog::~NewFileDialog()
{
    delete m_pTypeListCtrl;
}


/*****************************************************************************/
/**
 * Connect events of this dialog to handlers.
 */
void NewFileDialog::ConnectEventHandlers(void)
{
    Connect(wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(NewFileDialog::OnButton));
}


/*****************************************************************************/
void NewFileDialog::OnButton(wxCommandEvent& event)
{
    long item;

    m_pSelectedFormatInfo = NULL;

    if (event.GetId() == wxID_OK)
    {
        item = m_pTypeListCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if (item != -1)
        {
            m_pSelectedFormatInfo = (const FormatInfo*)
                m_pTypeListCtrl->GetItemData(item);
        }
    }
    event.Skip();
}
