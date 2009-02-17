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

#include <wx/menu.h>
#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "MCApp.h"
#include "MCMainFrame.h"
#include "MCDoc.h"

#include "MCToolDots.h"
#include "MCToolFreehand.h"
#include "MCToolLines.h"
#include "MCToolFill.h"
#include "MCToolCloneBrush.h"

IMPLEMENT_APP(MCApp);

/*****************************************************************************/
MCApp::MCApp()
    : m_pDocManager(NULL)
    , m_pMainFrame(NULL)
    , m_idDrawingTool(0)
    , m_listTools()
{
}

/*****************************************************************************/
MCApp::~MCApp()
{
}

/*****************************************************************************/
bool MCApp::OnInit()
{
    wxInitAllImageHandlers();

    m_pDocManager = new wxDocManager;

#if 0
    new wxDocTemplate(
        m_pDocManager, _T("Koala Image"), _T("*.koa"), _T(""), _T("koa"), _T("MCDoc"), _T("MCView"),
          CLASSINFO(MCDoc), CLASSINFO(MCView));
    new wxDocTemplate(
        m_pDocManager, _T("Amica Paint Image"), _T("*.ami"), _T(""), _T("ami"), _T("MCDoc"), _T("MCView"),
          CLASSINFO(MCDoc), CLASSINFO(MCView));
    new wxDocTemplate(
        m_pDocManager, _T("All Files"), _T("*"), _T(""), _T("ami"), _T("MCDoc"), _T("MCView"),
          CLASSINFO(MCDoc), CLASSINFO(MCView));
#endif

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
 * Return a pointer to the currently active tool. NULL if there is no tool
 * chosen.
 */
MCToolBase* MCApp::GetActiveDrawingTool()
{
    std::list<MCToolBase*>::iterator i;

    for (i = m_listTools.begin(); i != m_listTools.end(); ++i)
    {
        if ((*i)->GetToolId() == m_idDrawingTool)
            return *i;
    }

    return NULL;
}

/*****************************************************************************/
wxBitmap MCApp::GetBitmap(const wxString& dir, const wxString& name)
{
    wxStandardPaths paths;

    // Find out the path of our images
    wxFileName fileName(paths.GetExecutablePath());

    fileName.AppendDir(wxT("res"));
    fileName.AppendDir(dir);
    fileName.SetFullName(name);

    return wxBitmap(wxImage(fileName.GetFullPath(), wxBITMAP_TYPE_PNG));
}
