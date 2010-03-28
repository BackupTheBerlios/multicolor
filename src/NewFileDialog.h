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

#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <wx/dialog.h>
#include <wx/listctrl.h>

class FormatInfo;

class NewFileDialog : public wxDialog
{
public:
    NewFileDialog(wxWindow* parent);
    virtual ~NewFileDialog();

    void OnButton(wxCommandEvent& event);

    const FormatInfo* GetSelectedFormatInfo() const;

protected:
    void ConnectEventHandlers(void);
    wxListCtrl* m_pTypeListCtrl;

    const FormatInfo* m_pSelectedFormatInfo;
};

/*****************************************************************************/
/**
 * Return the selevcted FormatInfo or NULL if nothing has been selected.
 */
const inline FormatInfo* NewFileDialog::GetSelectedFormatInfo() const
{
    return m_pSelectedFormatInfo;
}

#endif // NEWFILEDIALOG_H
