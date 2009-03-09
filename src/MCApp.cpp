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

#include "MCApp.h"
#include "MCMainFrame.h"
#include "MCDoc.h"
#include "MCToolPanel.h"

#include "MCToolDots.h"
#include "MCToolFreehand.h"
#include "MCToolLines.h"
#include "MCToolFill.h"
#include "MCToolCloneBrush.h"
#include "MCToolColorPicker.h"

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
    wxInitAllImageHandlers();

    m_pMainFrame = new MCMainFrame(m_pMainFrame, _("MultiColor"));

    m_pMainFrame->Show();
    SetTopWindow(m_pMainFrame);

    AllocateTools();

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
    m_listTools.push_back(new MCToolDots);
    m_listTools.push_back(new MCToolFreehand);
    m_listTools.push_back(new MCToolLines);
    m_listTools.push_back(new MCToolFill);
    m_listTools.push_back(new MCToolCloneBrush);
    m_listTools.push_back(new MCToolColorPicker);
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
MCToolBase* MCApp::GetDrawingTool(int idTool /* = 0 */)
{
    std::list<MCToolBase*>::iterator i;

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
 * search in $(X)/res first and then in $(X)/../share/multicolor.
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
        fileName.AppendDir(wxT("multicolor"));
        fileName.AppendDir(wxT("res"));
        fileName.AppendDir(dir);
        fileName.SetFullName(name);
    }

    return wxImage(fileName.GetFullPath(), wxBITMAP_TYPE_PNG);
}


/*****************************************************************************/
/*
 * Load a bitmap from our ressources. If our executable is located in $(X),
 * search in $(X)/res first and then in $(X)/../share/multicolor.
 */
wxBitmap MCApp::GetBitmap(const wxString& dir, const wxString& name)
{
    return wxBitmap(GetImage(dir, name));
}


/*****************************************************************************/
/*
 * Set the active document which shall be used to update the preview
 * and so on from now. NULL means nothing to draw.
 */
void MCApp::SetActiveDoc(MCDoc* pDoc)
{
    m_pMainFrame->GetToolPanel()->SetActiveDoc(pDoc);
}

/*****************************************************************************/
/*
 * Set the name of the given document to the given value.
 * This is done in this strange way so the right window titles can be set
 * even if there is no simple relationship between them.
 */
void MCApp::SetDocName(const MCDoc* pDoc, const wxString name)
{
    m_pMainFrame->SetDocName(pDoc, name);
}
