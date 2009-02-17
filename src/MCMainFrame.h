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

#ifndef MCMAINFRAME_H
#define MCMAINFRAME_H

#include <wx/docmdi.h>

class PalettePanel;
class MCDoc;

class MCMainFrame: public wxMDIParentFrame
{
public:
    MCMainFrame(wxFrame* parent,
            const wxString& title);

    PalettePanel* GetPalettePanel();
    MCDoc* GetActiveDocument();

protected:
    void InitToolBar();
    void InitMenuBar();

    void OnNew(wxCommandEvent &event);

    void OnOpen(wxCommandEvent &event);

    void OnUpdateSave(wxUpdateUIEvent& event);
    void OnSave(wxCommandEvent& event);

    void OnUpdateSaveAs(wxUpdateUIEvent& event);
    void OnSaveAs(wxCommandEvent& event);

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUndo(wxCommandEvent& event);

    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnRedo(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTool(wxCommandEvent& event);

    void OnZoom(wxCommandEvent& event);
    void OnTVMode(wxCommandEvent& event);

    void OnUpdateZoomIn(wxUpdateUIEvent& event);
    void OnUpdateZoomOut(wxUpdateUIEvent& event);
    void OnUpdateZoom(wxUpdateUIEvent& event);
    void OnUpdateTVMode(wxUpdateUIEvent& event);

    PalettePanel* m_pPalettePanel;
};

/*****************************************************************************/
inline PalettePanel* MCMainFrame::GetPalettePanel()
{
    return m_pPalettePanel;
}


#endif // MCMAINFRAME_H
