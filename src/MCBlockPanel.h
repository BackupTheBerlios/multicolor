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

#include <wx/panel.h>
#include <wx/stattext.h>
#include "MCBlock.h"

class MCBlockPanel : public wxPanel
{
public:
    MCBlockPanel(wxWindow* pParent);
    virtual ~MCBlockPanel();

    void ShowBlock(MCDoc* pDoc, unsigned x, unsigned y);

protected:
    static const int m_nPixelHeight = 8;

    /* 8x4 Panels for Pixels */
    wxPanel* m_apPixelPanel[MCBLOCK_HEIGHT][MCBLOCK_WIDTH];

    /* 4 Colors */
    wxStaticText* m_apUsageStaticText[4];
    wxPanel* m_apUsagePanel[4];
};

#endif /* MCBLOCKPANEL_H_ */
