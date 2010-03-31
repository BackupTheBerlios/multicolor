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

#ifdef __WXMAC__
#include <ApplicationServices/ApplicationServices.h>
#endif // __WXMAC__

#include <wx/menu.h>
#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/cmdline.h>

#include "MCApp.h"
#include "MCMainFrame.h"
#include "MCDoc.h"
#include "ToolPanel.h"

#include "ToolDots.h"
#include "ToolFreehand.h"
#include "ToolLines.h"
#include "ToolFill.h"
#include "ToolCloneBrush.h"
#include "ToolColorPicker.h"

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    {
        wxCMD_LINE_PARAM,  NULL, NULL, wxT("image file"),
        wxCMD_LINE_VAL_STRING,
        wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL
    },
    { wxCMD_LINE_NONE }
};

IMPLEMENT_APP(MCApp);

/*****************************************************************************/
MCApp::MCApp()
    : m_pMainFrame(NULL)
    , m_idDrawingTool(0)
    , m_listTools()
{
#ifdef __WXMAC__
    ProcessSerialNumber psn;
    GetCurrentProcess(&psn);
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
#endif // __WXMAC__
}

/*****************************************************************************/
MCApp::~MCApp()
{
}

/*****************************************************************************/
bool MCApp::OnInit()
{
    size_t i;

    wxCmdLineParser cmdLineParser(cmdLineDesc, argc, argv);

    if (cmdLineParser.Parse() != 0)
        return false;

    wxInitAllImageHandlers();

    m_pMainFrame = new MCMainFrame(m_pMainFrame, _("MultiColor"));

    m_pMainFrame->Show();
    SetTopWindow(m_pMainFrame);

    AllocateTools();

    // open all files given on the command line
    for (i = 0; i < cmdLineParser.GetParamCount(); ++i)
    {
        m_pMainFrame->LoadDoc(cmdLineParser.GetParam(i));
    }

    return true;
}

/*****************************************************************************/
/*
 * Allocate all drawing tools.
 *
 * Only called from MCApp::MCApp.
 */
void MCApp::AllocateTools()
{
    m_listTools.push_back(new ToolDots);
    m_listTools.push_back(new ToolFreehand);
    m_listTools.push_back(new ToolLines);
    m_listTools.push_back(new ToolFill);
    m_listTools.push_back(new ToolCloneBrush);
    m_listTools.push_back(new ToolColorPicker);
}

/*****************************************************************************/
/*
 * Free all drawing tools.
 *
 * Only called from MCApp::~MCApp.
 */
void MCApp::FreeTools()
{
    while (m_listTools.size())
    {
        delete m_listTools.back();
        m_listTools.pop_back();
    }
}

/*****************************************************************************/
/*
 * Return a pointer to the given or the currently active tool.
 * If the argument is 0, the currently active tool is searched. Otherwise
 * the given ID is used.
 *
 * Returns NULL if there is no tool chosen.
 */
ToolBase* MCApp::GetDrawingTool(int idTool /* = 0 */)
{
    std::list<ToolBase*>::iterator i;

    if (idTool == 0)
    {
        idTool = m_idDrawingTool;
    }

    for (i = m_listTools.begin(); i != m_listTools.end(); ++i)
    {
        if ((*i)->GetToolId() == idTool)
            return *i;
    }

    return NULL;
}


/*****************************************************************************/
/*
 * Load an image from our ressources. If our executable is located in $(X),
 * search in $(X)/res first and then in $(X)/../share/MultiColor.
 */
wxImage MCApp::GetImage(const wxString& dir, const wxString& name)
{
    wxStandardPaths paths;

    // Find out the path of our images
    wxFileName fileName(paths.GetExecutablePath());

    fileName.AppendDir(wxT("res"));
    fileName.AppendDir(dir);
    fileName.SetFullName(name);

    if (!fileName.IsFileReadable())
    {
        fileName.Assign(paths.GetExecutablePath());
        fileName.RemoveLastDir();
        fileName.AppendDir(wxT("share"));
        fileName.AppendDir(wxT("MultiColor"));
        fileName.AppendDir(wxT("res"));
        fileName.AppendDir(dir);
        fileName.SetFullName(name);
    }

    return wxImage(fileName.GetFullPath(), wxBITMAP_TYPE_PNG);
}


/*****************************************************************************/
/**
 * Load a bitmap from our ressources. If our executable is located in $(X),
 * search in $(X)/res first and then in $(X)/../share/multicolor.
 */
wxBitmap MCApp::GetBitmap(const wxString& dir, const wxString& name)
{
    return wxBitmap(GetImage(dir, name));
}


/*****************************************************************************/
/**
 * Set the active document which shall be used to update the preview
 * and so on from now. NULL means nothing to draw.
 */
void MCApp::SetActiveDoc(DocBase* pDoc)
{
    m_pMainFrame->GetToolPanel()->SetActiveDoc(pDoc);
}

/*****************************************************************************/
/*
 * Set the name of the given document to the given value.
 * This is done in this strange way so the right window titles can be set
 * even if there is no simple relationship between them.
 */
void MCApp::SetDocName(const DocBase* pDoc, const wxString name)
{
    m_pMainFrame->SetDocName(pDoc, name);
}
