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

#include <wx/frame.h>

class ToolPanel;
class wxNotebook;

#define MC_MAX_ZOOM 16

class PalettePanel;
class DocBase;
class MCCanvas;

class MCMainFrame: public wxFrame
{
public:
    MCMainFrame(wxFrame* parent,
            const wxString& title);

    ToolPanel* GetToolPanel();
    DocBase* GetActiveDoc();
    MCCanvas* GetActiveCanvas();
    void ShowMousePos(int x, int y);
    void SetDocName(const DocBase* pDoc, const wxString stringName);
    void LoadDoc(const wxString& name);
    void FixFocus();

protected:
    void InitToolBar();
    void InitMenuBar();

    void OnPageChanged(wxCommandEvent &event);
    void OnFocus(wxFocusEvent& event);

    void OnNew(wxCommandEvent &event);

    void OnOpen(wxCommandEvent &event);

    void OnUpdateSave(wxUpdateUIEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSaveAs(wxCommandEvent& event);
    void OnFileClose(wxCommandEvent &event);

    void OnClose(wxCloseEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnUpdateUndo(wxUpdateUIEvent& event);
    void OnUndo(wxCommandEvent& event);

    void OnUpdateRedo(wxUpdateUIEvent& event);
    void OnRedo(wxCommandEvent& event);

    void OnAbout(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);

    void OnUpdateTool(wxUpdateUIEvent& event);
    void OnTool(wxCommandEvent& event);

    void OnZoom(wxCommandEvent& event);
    void OnTVMode(wxCommandEvent& event);

    void OnUpdateZoomIn(wxUpdateUIEvent& event);
    void OnUpdateZoomOut(wxUpdateUIEvent& event);
    void OnUpdateZoom(wxUpdateUIEvent& event);
    void OnUpdateTVMode(wxUpdateUIEvent& event);

    void OnKeyDown(wxKeyEvent& event);

    ToolPanel*      m_pToolPanel;
    wxNotebook*     m_pNotebook;
};

/*****************************************************************************/
inline ToolPanel* MCMainFrame::GetToolPanel()
{
    return m_pToolPanel;
}

#endif // MCMAINFRAME_H
