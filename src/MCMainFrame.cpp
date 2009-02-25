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

#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/filedlg.h>

#include "MCApp.h"
#include "MCDoc.h"
#include "MCMainFrame.h"
#include "MCChildFrame.h"
#include "PalettePanel.h"

/*****************************************************************************/
MCMainFrame::MCMainFrame(wxFrame* parent, const wxString& title)
    : wxMDIParentFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
    , m_pToolPanel(NULL)
{
    m_pToolPanel = new MCToolPanel(this);

    InitMenuBar();
    InitToolBar();

    CreateStatusBar(2);
    SetStatusText(_("?!"), 0);
    SetStatusText(_("!?"), 1);

    Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnNew));

    Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnOpen));

    Connect(wxID_SAVE, wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(MCMainFrame::OnUpdateSave));
    Connect(wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnSave));

    Connect(wxID_SAVEAS, wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(MCMainFrame::OnUpdateSave));
    Connect(wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnSaveAs));

    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(MCMainFrame::OnClose));
	Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnQuit));
	Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnAbout));
	Connect(wxEVT_SIZE, wxSizeEventHandler(MCMainFrame::OnSize));

    Connect(wxID_UNDO, wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(MCMainFrame::OnUpdateUndo));
    Connect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnUndo));

    Connect(wxID_REDO, wxEVT_UPDATE_UI,
            wxUpdateUIEventHandler(MCMainFrame::OnUpdateRedo));
    Connect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnRedo));

    Connect(MC_ID_ZOOM_1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(MC_ID_ZOOM_2, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(MC_ID_ZOOM_4, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(MC_ID_ZOOM_8, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(wxID_ZOOM_IN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(wxID_ZOOM_OUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(MC_ID_TV_MODE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTVMode));

    Connect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnUndo));
    Connect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnRedo));

    Connect(MC_ID_ZOOM_1, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_2, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_4, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_8, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(wxID_ZOOM_IN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoomIn));
    Connect(wxID_ZOOM_OUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoomOut));
    Connect(MC_ID_TV_MODE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTVMode));

    Connect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateUndo));
    Connect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateRedo));

	Connect(MC_ID_TOOL_DOTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_FREEHAND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
	Connect(MC_ID_TOOL_LINES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_FILL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_CLONE_BRUSH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
}


/*****************************************************************************/
void MCMainFrame::InitToolBar()
{
    wxToolBar*      pToolBar;
    wxBitmap        bitmap;

    pToolBar = CreateToolBar();
	pToolBar->SetToolBitmapSize(wxSize(24, 24));

    pToolBar->AddTool(wxID_NEW, _T("New"),
            MCApp::GetBitmap(wxT("24x24"), wxT("filenew.png")), _T("New file"));
    pToolBar->AddTool(wxID_OPEN, _T("Open"),
            MCApp::GetBitmap(wxT("24x24"), wxT("fileopen.png")), _T("Open file"));
    pToolBar->AddTool(wxID_SAVE, _T("Save"),
            MCApp::GetBitmap(wxT("24x24"), wxT("filesave.png")), _T("Save file"));

    pToolBar->AddSeparator();

    pToolBar->AddTool(wxID_UNDO, _T("Undo"),
            MCApp::GetBitmap(wxT("24x24"), wxT("undo.png")), _T("Undo"));
    pToolBar->AddTool(wxID_REDO, _T("Redo"),
            MCApp::GetBitmap(wxT("24x24"), wxT("redo.png")), _T("Redo"));

    pToolBar->AddSeparator();

    pToolBar->AddTool(wxID_ZOOM_OUT, _T("Zoom out"),
            MCApp::GetBitmap(wxT("24x24"), wxT("viewmag-.png")), _T("Zoom out"));

    pToolBar->AddTool(wxID_ZOOM_IN, _T("Zoom in"),
            MCApp::GetBitmap(wxT("24x24"), wxT("viewmag+.png")), _T("Zoom in"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("tv.png"));
    pToolBar->AddCheckTool(MC_ID_TV_MODE, _T("TV mode"),
            bitmap, bitmap, _T("Blur the image a bit"));

    pToolBar->AddSeparator();

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("dots.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_DOTS, _T("Draw dots"),
            bitmap, bitmap, _T("Draw single dots"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("freeline_unfilled.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_FREEHAND, _T("Freehand lines"),
            bitmap, bitmap, _T("Draw freehand lines"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("line.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_LINES, _T("Lines"),
            bitmap, bitmap, _T("Draw lines"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("color_fill.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_FILL, _T("Flood fill"),
            bitmap, bitmap, _T("Fill an area"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("clone.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_CLONE_BRUSH, _T("Clone brush"),
            bitmap, bitmap, _T("Clone brush"));

    // to make sure it is consistent:
    pToolBar->ToggleTool(MC_ID_TOOL_DOTS, true);
    wxGetApp().SetActiveDrawingTool(MC_ID_TOOL_DOTS);

    pToolBar->Realize();
}


/*****************************************************************************/
void MCMainFrame::InitMenuBar()
{
    //// Make a menubar
    wxMenu *pFileMenu = new wxMenu;

    pFileMenu->Append(wxID_NEW, _T("&New..."));
    pFileMenu->Append(wxID_OPEN, _T("&Open..."));
    pFileMenu->Append(wxID_CLOSE, _T("&Close"));
    pFileMenu->Append(wxID_SAVE, _T("&Save"));
    pFileMenu->Append(wxID_SAVEAS, _T("Save &As..."));

    pFileMenu->AppendSeparator();
    pFileMenu->Append(wxID_EXIT, _T("E&xit"));

    wxMenu *pViewMenu = new wxMenu;
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_1, _T("Zoom &1:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_2, _T("Zoom &2:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_4, _T("Zoom &4:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_8, _T("Zoom &8:1"));
    pViewMenu->Append(wxID_ZOOM_IN, _T("Zoom &in"));
    pViewMenu->Append(wxID_ZOOM_OUT, _T("Zoom &out"));
    pViewMenu->AppendSeparator();
    pViewMenu->Append(MC_ID_TV_MODE, _T("&TV Mode"), _T("Blur the image a little bit"), wxITEM_CHECK);

    wxMenu *pEditMenu = new wxMenu;

    pEditMenu->Append(wxID_UNDO, _T("&Undo"));
    pEditMenu->Append(wxID_REDO, _T("&Redo"));

    wxMenu *pHelpMenu = new wxMenu;
    pHelpMenu->Append(wxID_ABOUT, _T("&About"));

    wxMenuBar *pMenuBar = new wxMenuBar;
    pMenuBar->Append(pFileMenu, _T("&File"));
    pMenuBar->Append(pEditMenu, _T("&Edit"));
    pMenuBar->Append(pViewMenu, _T("&View"));
    pMenuBar->Append(pHelpMenu, _T("&Help"));

    SetMenuBar(pMenuBar);
}


/*****************************************************************************/
/*
 * Create a new file.
 */
void MCMainFrame::OnNew(wxCommandEvent &event)
{
    MCDoc* pDoc = new MCDoc();
    MCChildFrame* pChildFrame = new MCChildFrame(pDoc, this, wxID_ANY, wxT("child frame"));
    pDoc->SetFrame(pChildFrame);
    pChildFrame->Show();
}


/*****************************************************************************/
void MCMainFrame::OnOpen(wxCommandEvent &event)
{
	wxString stringFilter;

	stringFilter.append(wxT("All image files (*.koa;*.ami)|*.koa;*.ami|"));
    stringFilter.append(wxT("Koala files (*.koa)|*.koa|"));
	stringFilter.append(wxT("Amica files (*.ami)|*.ami|"));
    stringFilter.append(wxT("All files (*)|*|"));

	wxFileDialog* pFileDialog = new wxFileDialog(
            this, wxT("Open File"), wxT(""), wxT(""), stringFilter,
            wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_FILE_MUST_EXIST);

    if (pFileDialog->ShowModal() == wxID_OK)
    {
        // Create a new document and load the file into it
        MCDoc* pDoc = new MCDoc();
        MCChildFrame* pChildFrame = new MCChildFrame(pDoc, this, wxID_ANY, wxT("child frame"));
        pDoc->SetFrame(pChildFrame);
        pDoc->Load(pFileDialog->GetPath());
        pChildFrame->Show();
    }
    delete pFileDialog;
}


/*****************************************************************************/
/*
 * "Save" and "Save as" can only be used if there is a document.
 */
void MCMainFrame::OnUpdateSave(wxUpdateUIEvent& event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();
    event.Enable(pDoc != NULL);
}


/*****************************************************************************/
/*
 * Save the current document using its current file name.
 */
void MCMainFrame::OnSave(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();


    if (pDoc)
    {
        if (pDoc->GetFileName().GetPath().Length() == 0)
        {
            OnSaveAs(event);
        }
        else
            pDoc->Save(wxString());
    }
}


/*****************************************************************************/
void MCMainFrame::OnSaveAs(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();
	wxString stringFilter;

    /* !!! Keep the filter list in sync with the code below !!! */
	stringFilter.append(wxT("All image files (*.koa;*.ami)|*.koa;*.ami|"));
    stringFilter.append(wxT("Koala files (*.koa)|*.koa|"));
	stringFilter.append(wxT("Amica files (*.ami)|*.ami|"));
    stringFilter.append(wxT("All files (*)|*|"));
    /* !!! Keep the filter list in sync with the code below !!! */

    wxFileDialog* pFileDialog = new wxFileDialog(
            this, wxT("Save File"), wxT(""), wxT(""), stringFilter,
			wxFD_SAVE | wxFD_CHANGE_DIR | wxFD_OVERWRITE_PROMPT);

    if (pDoc && (pFileDialog->ShowModal() == wxID_OK))
    {
        wxFileName name(pFileDialog->GetPath());

        // didn't the user supply an extension?
        if (name.GetExt() == wxT(""))
        {
            // then add .koa, except if he has .ami chosen
            if (pFileDialog->GetFilterIndex() == 2)
                name.SetExt(wxT("ami"));
            else
                name.SetExt(wxT("koa"));
        }

        // Try to save the file
        pDoc->Save(name.GetFullPath());
    }
    delete pFileDialog;
}


/*****************************************************************************/
void MCMainFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}


/*****************************************************************************/
void MCMainFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}


/*****************************************************************************/
/*
 * Update Undo button.
 */
void MCMainFrame::OnUpdateUndo(wxUpdateUIEvent& event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();
    event.Enable(pDoc && pDoc->CanUndo());
}


/*****************************************************************************/
/*
 * Undo.
 */
void MCMainFrame::OnUndo(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();

    if (pDoc)
        pDoc->Undo();
}


/*****************************************************************************/
/*
 * Update Redo button.
 */
void MCMainFrame::OnUpdateRedo(wxUpdateUIEvent& event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();
    event.Enable(pDoc && pDoc->CanRedo());
}


/*****************************************************************************/
/*
 * Redo.
 */
void MCMainFrame::OnRedo(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDocument();

    if (pDoc)
        pDoc->Redo();
}


/*****************************************************************************/
/*
 * Get a pointer to the active document or NULL.
 */
MCDoc* MCMainFrame::GetActiveDocument()
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();
    if (pChild)
    {
        return pChild->GetDocument();
    }
    return NULL;
}

/*****************************************************************************/
void MCMainFrame::OnSize(wxSizeEvent& event)
{
    int w, h, minWidth;
    GetClientSize(&w, &h);

    minWidth = m_pToolPanel->GetMinWidth();
    m_pToolPanel->SetSize(0, 0, minWidth, h);
    GetClientWindow()->SetSize(minWidth, 0, w - minWidth, h);

    // FIXME: On wxX11, we need the MDI frame to process this
    // event, but on other platforms this should not
    // be done.
    // event.Skip();
}


/*****************************************************************************/
void MCMainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("MultiColor 0.1.1"), _("Welcome to..."));
}

/*****************************************************************************/
void MCMainFrame::OnTool(wxCommandEvent &event)
{
    wxGetApp().SetActiveDrawingTool(event.GetId());
}


/*****************************************************************************/
void MCMainFrame::OnZoom(wxCommandEvent& event)
{
    int nScale;
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();

    if (!pChild)
    {
        return;
    }

    nScale = pChild->GetScale();

    switch (event.GetId())
    {
    case MC_ID_ZOOM_1:
        nScale = 1;
        break;

    case MC_ID_ZOOM_2:
        nScale = 2;
        break;

    case MC_ID_ZOOM_4:
        nScale = 4;
        break;

    case MC_ID_ZOOM_8:
        nScale = 8;
        break;

    case wxID_ZOOM_IN:
        if (nScale < 8)
        {
            nScale *= 2;
        }
        break;

    case wxID_ZOOM_OUT:
        if (nScale > 1)
        {
            nScale /= 2;
        }
        break;
    }

    pChild->SetScale(nScale);
}

/*****************************************************************************/
void MCMainFrame::OnTVMode(wxCommandEvent& event)
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();
    if (pChild)
    {
        pChild->SetTVMode(event.IsChecked());
    }
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoomIn(wxUpdateUIEvent& event)
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();
    event.Enable(pChild ? (pChild->GetScale() < 8) : false);
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoomOut(wxUpdateUIEvent& event)
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();
    event.Enable(pChild ? (pChild->GetScale() > 1) : false);
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoom(wxUpdateUIEvent& event)
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();

    if (pChild)
    {
        event.Enable(true);
        switch (pChild->GetScale())
        {
        case 1:
            event.Check(event.GetId() == MC_ID_ZOOM_1);
            break;

        case 2:
            event.Check(event.GetId() == MC_ID_ZOOM_2);
            break;

        case 4:
            event.Check(event.GetId() == MC_ID_ZOOM_4);
            break;

        case 8:
            event.Check(event.GetId() == MC_ID_ZOOM_8);
            break;
        }
    }
    else
    {
        event.Enable(false);
    }
}


/*****************************************************************************/
/*
 * Update the state of the "emulate TV" button.
 */
void MCMainFrame::OnUpdateTVMode(wxUpdateUIEvent& event)
{
    MCChildFrame* pChild = (MCChildFrame*) GetActiveChild();
    if (pChild)
    {
        event.Enable(true);
        event.Check(pChild->GetTVMode());
    }
    else
    {
        event.Enable(false);
        event.Check(false);
    }
}
