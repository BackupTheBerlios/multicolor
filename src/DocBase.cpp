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

#include <wx/string.h>

#include "DocRenderer.h"
#include "DocBase.h"
#include "BitmapBase.h"
#include "MCApp.h"


/// This contains a number for unnamed documents
unsigned DocBase::m_nDocNumber;

/******************************************************************************/
/**
 * Constructor.
 */
DocBase::DocBase() :
    m_fileName(),
    m_bModified(false)
{
    m_fileName.SetName(wxString::Format(_T("unnamed%d"), ++m_nDocNumber));
}


/******************************************************************************/
/*
 * Add the renderer to the list of renderers showing this document.
 * Make sure it is only in the list once.
 */
void DocBase::AddRenderer(DocRenderer* pRenderer)
{
    m_listDocRenderers.remove(pRenderer);
    m_listDocRenderers.push_back(pRenderer);
}


/******************************************************************************/
/*
 * Remove the renderer from the list if it is in there.
 */
void DocBase::RemoveRenderer(DocRenderer* pRenderer)
{
    m_listDocRenderers.remove(pRenderer);
}

/*****************************************************************************/
/**
 * Mark the document modified/unmodified.
 */
void DocBase::Modify(bool bModified)
{
    wxString str;

    m_bModified = bModified;

    if (bModified)
        str = wxT("*");

    str.Append(m_fileName.GetFullName());

    // todo: Is this the right place?
    wxGetApp().SetDocName(this, str);
}


/******************************************************************************/
/**
 * Refresh the dirty rectangle in all renderers associated with this document.
 * Then reset the dirty rectangle.
 */
void DocBase::RefreshDirty()
{
    wxRect dirty;

    dirty = GetBitmap()->GetDirtyRect();
    if (dirty.GetRight() >= 0)
    {
        Refresh(dirty.GetLeft(), dirty.GetTop(),
                dirty.GetRight(), dirty.GetBottom());
        GetBitmap()->ResetDirty();
    }
}


/******************************************************************************/
/*
 * Set the current mouse position to a point in this document. This allows
 * all Renderers to display the current position. The coordinates must be
 * in bitmap coordinates.
 */
void DocBase::SetMousePos(int x, int y)
{
    std::list<DocRenderer*>::iterator i;

    m_pointMousePos.x = x;
    m_pointMousePos.y = y;

    wxGetApp().GetMainFrame()->ShowMousePos(x, y);
    for (i  = m_listDocRenderers.begin();
         i != m_listDocRenderers.end();
         ++i)
    {
        (*i)->OnDocMouseMoved(x, y);
    }
}
