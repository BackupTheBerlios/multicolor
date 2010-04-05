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

#include "FormatInfo.h"
#include "MCApp.h"
#include "DocBase.h"
#include "MCMainFrame.h"
#include "MCCanvas.h"
#include "PalettePanel.h"
#include "NewFileDialog.h"

/*
 * On Windows the png icons with alpha channel look very strange in the toolbar
 * when the tool is disabled (They are not grayed out but appear black). This
 * happens with wxMSW 2.8.9. That's why we leave them always active.
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
    m_pToolPanel = new ToolPanel(this);
    m_pNotebook = new wxNotebook(this, wxID_ANY);

    InitMenuBar();
    InitToolBar();

    CreateStatusBar(2);
    SetStatusText(_("?!"), 0);
    SetStatusText(_("!?"), 1);

    Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxCommandEventHandler(MCMainFrame::OnPageChanged));
    Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(MCMainFrame::OnFocus));

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

    Connect(wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(MCMainFrame::OnFileClose));

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
    Connect(MC_ID_ZOOM_16, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(wxID_ZOOM_IN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(wxID_ZOOM_OUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnZoom));
    Connect(MC_ID_TV_MODE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTVMode));

    Connect(wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnUndo));
    Connect(wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnRedo));

    Connect(MC_ID_ZOOM_1, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_2, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_4, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_8, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(MC_ID_ZOOM_16, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoom));
    Connect(wxID_ZOOM_IN, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoomIn));
    Connect(wxID_ZOOM_OUT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateZoomOut));
    Connect(MC_ID_TV_MODE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTVMode));

    Connect(wxID_UNDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateUndo));
    Connect(wxID_REDO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateRedo));

    Connect(MC_ID_TOOL_COLOR_PICKER, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
	Connect(MC_ID_TOOL_DOTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_FREEHAND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
	Connect(MC_ID_TOOL_LINES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_FILL, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));
    Connect(MC_ID_TOOL_CLONE_BRUSH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MCMainFrame::OnTool));

    Connect(MC_ID_TOOL_COLOR_PICKER, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));
    Connect(MC_ID_TOOL_DOTS, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));
    Connect(MC_ID_TOOL_FREEHAND, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));
    Connect(MC_ID_TOOL_LINES, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));
    Connect(MC_ID_TOOL_FILL, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));
    Connect(MC_ID_TOOL_CLONE_BRUSH, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MCMainFrame::OnUpdateTool));

    Connect(wxEVT_KEY_DOWN, wxKeyEventHandler(MCMainFrame::OnKeyDown));
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

    pToolBar->AddTool(wxID_ZOOM_IN, _T("Zoom in"),
            MCApp::GetBitmap(wxT("24x24"), wxT("zoomin.png")), _T("Zoom in"));

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("tv.png"));
    pToolBar->AddCheckTool(MC_ID_TV_MODE, _T("TV mode"),
            bitmap, bitmap, _T("Blur the image a bit"));

    pToolBar->AddSeparator();

    bitmap = MCApp::GetBitmap(wxT("24x24"), wxT("colorpicker.png"));
    pToolBar->AddRadioTool(MC_ID_TOOL_COLOR_PICKER, _T("Pick colors"),
            bitmap, bitmap, _T("Pick colors"));

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
    wxGetApp().SetDrawingTool(MC_ID_TOOL_FREEHAND);

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

    wxMenu *pEditMenu = new wxMenu;

    pEditMenu->Append(wxID_UNDO, _T("&Undo\tCtrl+Z"));
    pEditMenu->Append(wxID_REDO, _T("&Redo\tShift+Ctrl+Z"));

    wxMenu* pToolsMenu = new wxMenu;
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_COLOR_PICKER, _T("Color &picker\tF1"));
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_DOTS, _T("&Dots\tF2"));
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_FREEHAND, _T("&Freehand\tF3"));
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_LINES, _T("&Lines\tF4"));
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_FILL, _T("Fl&ood fill\tF5"));
    pToolsMenu->AppendRadioItem(MC_ID_TOOL_CLONE_BRUSH, _T("&Clone brush\tF6"));

    wxMenu* pViewMenu = new wxMenu;
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_1, _T("Zoom &1:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_2, _T("Zoom &2:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_4, _T("Zoom &4:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_8, _T("Zoom &8:1"));
    pViewMenu->AppendRadioItem(MC_ID_ZOOM_16, _T("Zoom 1&6:1"));
    pViewMenu->Append(wxID_ZOOM_IN, _T("Zoom &in"));
    pViewMenu->Append(wxID_ZOOM_OUT, _T("Zoom &out"));
    pViewMenu->AppendSeparator();
    pViewMenu->Append(MC_ID_TV_MODE, _T("&TV Mode"), _T("Blur the image a little bit"), wxITEM_CHECK);

    wxMenu *pHelpMenu = new wxMenu;
    pHelpMenu->Append(wxID_ABOUT, _T("&About"));

    wxMenuBar *pMenuBar = new wxMenuBar;
    pMenuBar->Append(pFileMenu, _T("&File"));
    pMenuBar->Append(pEditMenu, _T("&Edit"));
    pMenuBar->Append(pToolsMenu, _T("&Tools"));
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
#ifdef N2C
    int bcpos(320*(y/8)+(x/4)*8); int bpos=bcpos+(y&7); int cpos=(40*(y/8)+(x/4));
    wxString strPosition(wxString::Format(wxT("%d:%d\t$%04x\t$%04x\t$%04x"), x, y, cpos,bcpos,bpos));
#else
    wxString strPosition(wxString::Format(wxT("%d:%d"), x, y));
#endif
    SetStatusText(strPosition, 1);
}


/*****************************************************************************/
/*
 * Set the name of the given document to the given value.
 * This is done in this strange way so the right window titles can be set
 * even if there is no simple relationship between them.
 */
void MCMainFrame::SetDocName(const DocBase* pDoc, const wxString name)
{
    MCCanvas* pCanvas;
    size_t n;
    size_t nPages = m_pNotebook->GetPageCount();

    // Search for page with this Document and change its title
    for (n = 0; n < nPages; ++n)
    {
        pCanvas = (MCCanvas*) m_pNotebook->GetPage(n);
        if (pCanvas->GetDoc() == pDoc)
        {
            m_pNotebook->SetPageText(n, name);
        }
    }
}


/*****************************************************************************/
/*
 * Open a document with the given file name. This will be done in a new
 * document window (notebook page).
 */
void MCMainFrame::LoadDoc(const wxString& name)
{
    // Create a new document and load the file into it
    DocBase* pDoc = DocBase::Load(name);
    if (pDoc)
    {
        MCCanvas* pCanvas = new MCCanvas(m_pNotebook, 0, false);
        pCanvas->SetDoc(pDoc);
        m_pNotebook->AddPage(pCanvas, pDoc->GetFileName().GetFullName(), true);
        pCanvas->Show();
    }
}


/*****************************************************************************/
/*
 * Set the focus to the selected notebook page.
 */
void MCMainFrame::FixFocus()
{
    int nSelected = m_pNotebook->GetSelection();
    if (nSelected >= 0)
    {
        m_pNotebook->GetPage(nSelected)->SetFocus();
    }
}

/*****************************************************************************/
/*
 * The page has been changed. Update the active document and set the focus to
 * the canvas.
 */
void MCMainFrame::OnPageChanged(wxCommandEvent &event)
{
    int nSelected;
    MCCanvas* pCanvas;
    DocBase* pDoc;

    nSelected = m_pNotebook->GetSelection();
    if (nSelected < 0)
        return;

    pCanvas = (MCCanvas*) m_pNotebook->GetPage(nSelected);
    pDoc = pCanvas->GetDoc();

    wxGetApp().SetActiveDoc(pDoc);
    pCanvas->SetFocus();
}


/*****************************************************************************/
/*
 * Set the focus to the current notebook page each time our main window gets
 * the focus.
 *
 * On Windows after minimizing and restoring the window the shortcuts bound to
 * the canvas (e.g. Shift for color picker) didn't work anymore.
 */
void MCMainFrame::OnFocus(wxFocusEvent& event)
{
    FixFocus();
    event.Skip();
}


/*****************************************************************************/
/**
 * Create a new file.
 */
void MCMainFrame::OnNew(wxCommandEvent &event)
{
    DocBase* pDoc;
    NewFileDialog dlg(this);

    dlg.ShowModal();
    if (dlg.GetSelectedFormatInfo())
    {
        pDoc = dlg.GetSelectedFormatInfo()->Factory();

        MCCanvas* pCanvas = new MCCanvas(m_pNotebook, 0, false);
        pCanvas->SetDoc(pDoc);
        m_pNotebook->AddPage(pCanvas, pDoc->GetFileName().GetFullName(), true);
        pCanvas->Show();
    }
}


/*****************************************************************************/
void MCMainFrame::OnOpen(wxCommandEvent &event)
{
	wxString stringFilter;

    stringFilter = FormatInfo::GetFullFilterString();

	wxFileDialog* pFileDialog = new wxFileDialog(
            this, wxT("Open File"), wxT(""), wxT(""), stringFilter,
            wxFD_OPEN | wxFD_CHANGE_DIR | wxFD_FILE_MUST_EXIST);

    if (pFileDialog->ShowModal() == wxID_OK)
    {
        LoadDoc(pFileDialog->GetPath());
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
    DocBase* pDoc = GetActiveDoc();


    if (pDoc)
    {
        if (pDoc->GetFileName().GetPath().Length() == 0)
        {
            OnSaveAs(event);
        }
        else
            pDoc->Save(wxT(""));
    }
}


/*****************************************************************************/
void MCMainFrame::OnSaveAs(wxCommandEvent &event)
{
    DocBase* pDoc = GetActiveDoc();
	wxString stringFilter;
#if 0
    /* !!! Keep the filter list in sync with the code below !!! */
    stringFilter.append(wxT("All image files (*.koa;*.kla;*.ami)|*.koa;*.kla;*.ami|"));
    stringFilter.append(wxT("Koala files (*.koa;*.kla)|*.koa;*.kla|"));
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
            // then add .koa, except if .ami is selected
            if (pFileDialog->GetFilterIndex() == 2)
                name.SetExt(wxT("ami"));
            else
                name.SetExt(wxT("koa"));
        }

        // Try to save the file
        pDoc->Save(name.GetFullPath());
    }
    delete pFileDialog;
#endif
}


/*****************************************************************************/
void MCMainFrame::OnFileClose(wxCommandEvent &event)
{
    bool bReallyClose = false;
    int result;
    int nSelected;
    MCCanvas* pCanvas;
    DocBase* pDoc;

    nSelected = m_pNotebook->GetSelection();
    if (nSelected < 0)
        return;

    pCanvas = (MCCanvas*) m_pNotebook->GetPage(nSelected);
    pDoc = pCanvas->GetDoc();

    if (pDoc->IsModified())
    {
        wxString str;
        str.append(wxT("The document \""));
        str.append(pDoc->GetFileName().GetFullName());
        str.append(wxT("\" has been modified. Do you want to save your changes?"));

        result = wxMessageBox(str, wxT("Document modified"),
        wxYES_NO | wxCANCEL | wxICON_QUESTION, this);

        if (result == wxYES)
        {
            OnSave(event);
            bReallyClose = true;
        }
        else if (result == wxNO)
        {
            bReallyClose = true;
        }
        else /* CANCEL */
        {
            bReallyClose = false;
        }
    }
    else
    {
        bReallyClose = true;
    }

    if (bReallyClose)
    {
        m_pNotebook->DeletePage(nSelected);
        delete pDoc;
    }
}

/*****************************************************************************/
void MCMainFrame::OnClose(wxCloseEvent &event)
{
    bool bWasCanceled = false;
    wxCommandEvent evtDummy;
    unsigned nPages;

    while (!bWasCanceled &&
           (nPages = m_pNotebook->GetPageCount()) != 0)
    {
        OnFileClose(evtDummy);

        // If the number of pages did NOT change, the user pressed CANCEL
        if (nPages == m_pNotebook->GetPageCount())
        {
            bWasCanceled = true;
        }
    }

    if (!bWasCanceled)
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
    DocBase* pDoc = GetActiveDoc();
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
    DocBase* pDoc = GetActiveDoc();

    if (pDoc)
        pDoc->Undo();
}


/*****************************************************************************/
/*
 * Update Redo button.
 */
void MCMainFrame::OnUpdateRedo(wxUpdateUIEvent& event)
{
    DocBase* pDoc = GetActiveDoc();
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
    DocBase* pDoc = GetActiveDoc();

    if (pDoc)
        pDoc->Redo();
}


/*****************************************************************************/
/*
 * Get a pointer to the active document or NULL.
 */
DocBase* MCMainFrame::GetActiveDoc()
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
}


/*****************************************************************************/
void MCMainFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("MultiColor 0.2.1"), _("Welcome to..."));
}


/*****************************************************************************/
void MCMainFrame::OnUpdateTool(wxUpdateUIEvent &event)
{
    int id;
    ToolBase* pTool;

    pTool = wxGetApp().GetDrawingTool();
    id = pTool ? pTool->GetToolId() : -1;

    event.Enable(true);
    event.Check(event.GetId() == id);
}


/*****************************************************************************/
void MCMainFrame::OnTool(wxCommandEvent &event)
{
    wxGetApp().SetDrawingTool(event.GetId());
    MCCanvas::UpdateAllCursorTypes();
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


/*****************************************************************************/
void MCMainFrame::OnKeyDown(wxKeyEvent& event)
{
    int key = event.GetKeyCode();
    int nColor;

    if (key >= '1' && key <= '8')
    {
        if (event.GetModifiers() & wxMOD_SHIFT)
        {
            // colors 8..15
            nColor = key - '1' + 8;
        }
        else
        {
            // colors 0..7
            nColor = key - '1';
        }
        m_pToolPanel->GetPalettePanel()->SelectColor(
                nColor, (event.GetModifiers() & wxMOD_CONTROL) != 0);
    }
}

