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

#ifndef MCAPP_H
#define MCAPP_H

#include <wx/app.h>
#include <list>

#include "MCMainFrame.h"
#include "ToolPanel.h"

#define MC_MAX_FILE_BUFF_SIZE 0x10000


class PalettePanel;
class ToolBase;
class DocBase;
class MCChildFrame;

class MCApp : public wxApp
{
public:
    MCApp();
    virtual ~MCApp();
    virtual bool OnInit();

    static wxImage GetImage(const wxString& dir, const wxString& name);
    static wxBitmap GetBitmap(const wxString& dir, const wxString& name);

    void SetDrawingTool(int id);
    ToolBase* GetDrawingTool(int idTool = 0);

    void SetActiveDoc(DocBase* pDoc);
    void SetMousePos(int x, int y);
    void SetDocName(const DocBase* pDoc, const wxString stringName);

    MCMainFrame* GetMainFrame();
    PalettePanel* GetPalettePanel();

protected:
    MCMainFrame*    m_pMainFrame;

    int             m_idDrawingTool;
    std::list<ToolBase*> m_listTools;

private:
    void AllocateTools();
    void FreeTools();
};

DECLARE_APP(MCApp)

/*****************************************************************************/
inline void MCApp::SetDrawingTool(int id)
{
    m_idDrawingTool = id;
}

/*****************************************************************************/
inline PalettePanel* MCApp::GetPalettePanel()
{
    return m_pMainFrame->GetToolPanel()->GetPalettePanel();
}

/*****************************************************************************/
inline MCMainFrame* MCApp::GetMainFrame()
{
    return m_pMainFrame;
}


/*****************************************************************************/
enum MultiColorId
{
    MC_ID_ZOOM_1 = wxID_HIGHEST + 1,
    MC_ID_ZOOM_2,
    MC_ID_ZOOM_4,
    MC_ID_ZOOM_8,
    MC_ID_ZOOM_16,
    MC_ID_TV_MODE,

    MC_ID_TILE,
    MC_ID_CASCADE,
    MC_ID_NEW_VIEW,

    MC_ID_TOOL_DOTS,
    MC_ID_TOOL_FREEHAND,
    MC_ID_TOOL_LINES,
    MC_ID_TOOL_FILL,
    MC_ID_TOOL_CLONE_BRUSH,
    MC_ID_TOOL_COLOR_PICKER
};

#endif // MCAPP_H
