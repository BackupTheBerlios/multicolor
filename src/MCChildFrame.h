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

#ifndef MCCHILDFRAME_H
#define MCCHILDFRAME_H

#include <wx/mdi.h>

class MCCanvas;
class MCDoc;

class MCChildFrame : public wxMDIChildFrame
{
public:
    MCChildFrame(MCDoc* pDoc, wxMDIParentFrame* pParent, wxWindowID id, const wxString& title);
    virtual ~MCChildFrame();

    virtual void OnDraw(wxDC* pDC);  // overridden to draw this view
    //virtual bool OnCreate(wxDocument* pDoc, long flags);

    MCDoc* GetDocument();

    void SetMousePos(int x, int y);
    const wxPoint& GetMousePos();

    bool GetTVMode();
    void SetTVMode(bool b);
    int  GetScale();
    void SetScale(int nScale);

    void OnActivate(wxActivateEvent& event);

protected:
    MCDoc*      m_pDoc;

    MCCanvas*   m_pCanvas;
    int         m_nScale;

    bool        m_bTVMode;

    wxPoint     m_pointMousePos;
    bool        m_bMousePosDrawn;  /* true, wenn Mausposition momentan angezeigt */
};

/*****************************************************************************/
/*
 * Return the document associated with this Frame.
 */
inline MCDoc* MCChildFrame::GetDocument()
{
    return m_pDoc;
}

/*****************************************************************************/
/*
 *  Return m_bTVMode, this is true for TV emulation.
 */
inline bool MCChildFrame::GetTVMode()
{
    return m_bTVMode;
}

/*****************************************************************************/
/*
 * Return the current zoom factor.
 */
inline int MCChildFrame::GetScale()
{
    return m_nScale;
}


#endif /* MCCHILDFRAME_H */
