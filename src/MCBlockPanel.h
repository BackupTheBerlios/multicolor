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
#ifndef MCBLOCKPANEL_H_
#define MCBLOCKPANEL_H_

#include <wx/window.h>
#include <wx/timer.h>
#include <wx/dc.h>

#include "MCBlock.h"
#include "DocRenderer.h"

#define MCBLOCKPANEL_REFRESH_TIMER_ID 1
#define MCBLOCKPANEL_UPDATE_INTERVAL 200

class MCBlockPanel : public wxWindow, public DocRenderer
{
public:
    MCBlockPanel(wxWindow* pParent);
    virtual ~MCBlockPanel();

    virtual void OnDocChanged(int x1, int y1, int x2, int y2);
    virtual void OnDocMouseMoved(int x, int y);
    virtual void OnDocDestroy(DocBase* pDoc);

    void SetDoc(DocBase* pDoc);

protected:
    static const int m_nWTotal = 160;
    static const int m_nHTotal = 80;

    static const int m_nWBox = 8;
    static const int m_nHBox = 8;

    static const int m_nWBorder = (m_nHTotal - MCBLOCK_HEIGHT * m_nHBox) / 2;

    DocBase* m_pDoc;

    wxTimer  m_timerRefresh;

    void OnPaint(wxPaintEvent& event);
    void OnTimer(wxTimerEvent& event);
    void DrawBlock(wxDC* pDC, DocBase* pDoc, unsigned x, unsigned y);
};

#endif /* MCBLOCKPANEL_H_ */
