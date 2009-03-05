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

#include <wx/notebook.h>
#include <wx/toolbar.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/image.h>
#include <wx/filedlg.h>

#include "MCApp.h"
#include "MCDoc.h"
#include "MCMainFrame.h"
#include "MCCanvas.h"
#include "PalettePanel.h"

/*
 * On Windows the png icons with alpha channel look very strange in the toolbar
 * when the tool is disabled (They are not grayed out but appear black). This
 * happens at least with wxMSW 2.8.9. That's why we leave them always active.
 */
#ifdef __WXMSW__
#define MC_TOOLS_ALWAYS_ENABLED
#endif

/*****************************************************************************/
MCMainFrame::MCMainFrame(wxFrame* parent, const wxString& title) :
    wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)),
    m_pToolPanel(NULL),
    m_pNotebook(NULL)
{
    m_pToolPanel = new MCToolPanel(this);
    m_pNotebook = new wxNotebook(this, wxID_ANY);

    InitMenuBar();
    InitToolBar();

    CreateStatusBar(2);
    SetStatusText(_("?!"), 0);
    SetStatusText(_("!?"), 1);

    Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxCommandEventHandler(MCMainFrame::OnPageChanged));

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
            MCApp::GetBitmap(wxT("24x24"), wxT("zoomout.png")), _T("Zoom out"));

    pToolBar->AddTool(wxID_ZOOM_IN, _T("Zoom in\t+"),
            MCApp::GetBitmap(wxT("24x24"), wxT("zoomin.png")), _T("Zoom in"));

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
    // Make a menubar
    wxMenuItem* pItem;
    wxMenu*     pFileMenu = new wxMenu;

    pItem = new wxMenuItem(pFileMenu, wxID_NEW);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("filenew.png")));
    pFileMenu->Append(pItem);

    pItem = new wxMenuItem(pFileMenu, wxID_OPEN);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("fileopen.png")));
    pFileMenu->Append(pItem);

    pItem = new wxMenuItem(pFileMenu, wxID_CLOSE);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("fileclose.png")));
    pFileMenu->Append(pItem);

    pItem = new wxMenuItem(pFileMenu, wxID_SAVE);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("filesave.png")));
    pFileMenu->Append(pItem);

    pItem = new wxMenuItem(pFileMenu, wxID_SAVEAS);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("filesaveas.png")));
    pFileMenu->Append(pItem);

    pFileMenu->AppendSeparator();

    pItem = new wxMenuItem(pFileMenu, wxID_EXIT);
    pItem->SetBitmap(MCApp::GetBitmap(wxT("16x16"), wxT("quit.png")));
    pFileMenu->Append(pItem);

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
 * Show the current mouse position in the status bar
 */
void MCMainFrame::ShowMousePos(int x, int y)
{
    wxString strPosition(wxString::Format(wxT("%d:%d"), x, y));
    SetStatusText(strPosition, 1);
}

/*****************************************************************************/
/*
 * Create a new file.
 */
void MCMainFrame::OnPageChanged(wxCommandEvent &event)
{
    wxGetApp().SetActiveDoc(GetActiveDoc());
}

/*****************************************************************************/
/*
 * Create a new file.
 */
void MCMainFrame::OnNew(wxCommandEvent &event)
{
    MCDoc* pDoc = new MCDoc();
    MCCanvas* pCanvas = new MCCanvas(m_pNotebook, 0);
    pCanvas->SetDoc(pDoc);
    m_pNotebook->AddPage(pCanvas, pDoc->GetFileName().GetFullName(), true);
    pCanvas->Show();
}


/*****************************************************************************/
void MCMainFrame::OnOpen(wxCommandEvent &event)
{
	wxString stringFilter;

	stringFilter.append(wxT("All image files (*.koa;*.ami)|*.koa;*.ami|"));
    stringFilter.append(wxT("Koala files (*.koa)|*.koa|"));
	stringFilter.append(wxT("Amica files (*.ami)|*.ami|"));
    stringFilter.append(wxT("All files (*)|*"));

	wxFileDialog* pFileDialog = new wxFileDialog(
            this, wxT("Open File"), wxT(""), wxT(""), stringFilter,
            wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_FILE_MUST_EXIST);

    if (pFileDialog->ShowModal() == wxID_OK)
    {
        // Create a new document and load the file into it
        MCDoc* pDoc = new MCDoc();
        MCCanvas* pCanvas = new MCCanvas(m_pNotebook, 0);
        pCanvas->SetDoc(pDoc);
        m_pNotebook->AddPage(pCanvas, pDoc->GetFileName().GetFullName(), true);
        pDoc->Load(pFileDialog->GetPath());
        pCanvas->Show();
    }
    delete pFileDialog;
}


/*****************************************************************************/
/*
 * "Save" and "Save as" can only be used if there is a document.
 */
void MCMainFrame::OnUpdateSave(wxUpdateUIEvent& event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();
    event.Enable(pDoc != NULL);

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
/*
 * Save the current document using its current file name.
 */
void MCMainFrame::OnSave(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();


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
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();
	wxString stringFilter;

    /* !!! Keep the filter list in sync with the code below !!! */
	stringFilter.append(wxT("All image files (*.koa;*.ami)|*.koa;*.ami|"));
    stringFilter.append(wxT("Koala files (*.koa)|*.koa|"));
	stringFilter.append(wxT("Amica files (*.ami)|*.ami|"));
    stringFilter.append(wxT("All files (*)|*"));
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
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();
    event.Enable(pDoc && pDoc->CanUndo());

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
/*
 * Undo.
 */
void MCMainFrame::OnUndo(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();

    if (pDoc)
        pDoc->Undo();
}


/*****************************************************************************/
/*
 * Update Redo button.
 */
void MCMainFrame::OnUpdateRedo(wxUpdateUIEvent& event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();
    event.Enable(pDoc && pDoc->CanRedo());

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
/*
 * Redo.
 */
void MCMainFrame::OnRedo(wxCommandEvent &event)
{
    MCDoc* pDoc = (MCDoc*) GetActiveDoc();

    if (pDoc)
        pDoc->Redo();
}


/*****************************************************************************/
/*
 * Get a pointer to the active document or NULL.
 */
MCDoc* MCMainFrame::GetActiveDoc()
{
    MCCanvas* pCanvas = GetActiveCanvas();

    if (pCanvas)
    {
        return pCanvas->GetDoc();
    }
    return NULL;
}


/*****************************************************************************/
/*
 * Get the active Canvas or NULL if there is none.
 */
MCCanvas* MCMainFrame::GetActiveCanvas()
{
    int nSelected;

    nSelected = m_pNotebook->GetSelection();
    if (nSelected < 0)
        return NULL;

    return (MCCanvas*) m_pNotebook->GetPage(nSelected);
}


/*****************************************************************************/
void MCMainFrame::OnSize(wxSizeEvent& event)
{
    int w, h, wToolPanel;
    GetClientSize(&w, &h);

    wToolPanel = m_pToolPanel->GetMinWidth();
    m_pToolPanel->SetSize(0, 0, wToolPanel, h);

    m_pNotebook->SetSize(wToolPanel, 0, w - wToolPanel, h);

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
    MCCanvas* pCanvas = GetActiveCanvas();

    if (!pCanvas)
        return;

    nScale = pCanvas->GetScale();

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

    case MC_ID_ZOOM_16:
        nScale = 16;
        break;

    case wxID_ZOOM_IN:
        if (nScale < MC_MAX_ZOOM)
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

    pCanvas->SetScale(nScale);
}

/*****************************************************************************/
void MCMainFrame::OnTVMode(wxCommandEvent& event)
{
    MCCanvas* pCanvas = GetActiveCanvas();
    if (pCanvas)
    {
        pCanvas->SetEmulateTV(event.IsChecked());
    }
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoomIn(wxUpdateUIEvent& event)
{
    MCCanvas* pCanvas = GetActiveCanvas();
    event.Enable(pCanvas ? (pCanvas->GetScale() < MC_MAX_ZOOM) : false);

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoomOut(wxUpdateUIEvent& event)
{
    MCCanvas* pCanvas = GetActiveCanvas();
    event.Enable(pCanvas ? (pCanvas->GetScale() > 1) : false);

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
void MCMainFrame::OnUpdateZoom(wxUpdateUIEvent& event)
{
    MCCanvas* pCanvas = GetActiveCanvas();

    if (pCanvas)
    {
        event.Enable(true);
        switch (pCanvas->GetScale())
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

        case 16:
            event.Check(event.GetId() == MC_ID_ZOOM_16);
            break;
        }
    }
    else
    {
        event.Enable(false);
    }

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}


/*****************************************************************************/
/*
 * Update the state of the "emulate TV" button.
 */
void MCMainFrame::OnUpdateTVMode(wxUpdateUIEvent& event)
{
    MCCanvas* pCanvas = GetActiveCanvas();

    if (pCanvas)
    {
        event.Enable(true);
        event.Check(pCanvas->GetEmulateTV());
    }
    else
    {
        event.Enable(false);
        event.Check(false);
    }

#ifdef MC_TOOLS_ALWAYS_ENABLED
    event.Enable(true);
#endif
}
