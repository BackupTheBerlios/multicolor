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
#include <wx/file.h>
#include <wx/msgdlg.h>

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
    m_bModified(false),
    m_listUndo(),
    m_nRedoPos(0)
{
    m_fileName.SetName(wxString::Format(_T("unnamed%d"), ++m_nDocNumber));
}


/******************************************************************************/
/**
 * Destruktor.
 * Tell all my renderers that I'm going to go.
 */
DocBase::~DocBase()
{
    std::list<DocRenderer*>::iterator i;

    for (i = m_listDocRenderers.begin(); i != m_listDocRenderers.end(); ++i)
    {
        (*i)->OnDocDestroy(this);
    }
}


/******************************************************************************/
/**
 * Add the renderer to the list of renderers showing this document.
 * Make sure it is only in the list once.
 */
void DocBase::AddRenderer(DocRenderer* pRenderer)
{
    m_listDocRenderers.remove(pRenderer);
    m_listDocRenderers.push_back(pRenderer);
}


/******************************************************************************/
/**
 * Remove the renderer from the list if it is in there.
 */
void DocBase::RemoveRenderer(DocRenderer* pRenderer)
{
    m_listDocRenderers.remove(pRenderer);
}


/******************************************************************************/
/**
 * Refresh all renderers associated with this document.
 */
void DocBase::Refresh(int x1, int y1, int x2, int y2)
{
    std::list<DocRenderer*>::iterator i;

    // bring them to the right order
    GetBitmap()->SortAndClip(&x1, &y1, &x2, &y2);

    for (i = m_listDocRenderers.begin(); i != m_listDocRenderers.end(); ++i)
    {
        (*i)->OnDocChanged(x1, y1, x2, y2);
    }
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



/******************************************************************************/
/**
 * Save the current bitmap as undo step and mark the document as being
 * modified.
 */
void DocBase::PrepareUndo()
{
    unsigned i;

    Modify(true);

    // if we are not at the end of the undo list, discard the rest
    while (m_nRedoPos < m_listUndo.size())
    {
        delete m_listUndo.back();
        m_listUndo.pop_back();
    }

    // if the list is full, remove the first entry
    if (m_listUndo.size() >= MC_UNDO_LEN)
    {
        delete m_listUndo.front();
        m_listUndo.pop_front();
        --m_nRedoPos;
    }

    // append current state
    m_listUndo.push_back(GetBitmap()->Copy());
    m_nRedoPos++;
}

/******************************************************************************/
/**
 * Undo, if possible. Then refresh the the DocRenderers.
 */
void DocBase::Undo()
{
    std::list<BitmapBase*>::iterator it;
    int i;

    if (CanUndo())
    {
        // find the right position in the undo list
        m_nRedoPos--;
        it = m_listUndo.begin();
        for (i = m_nRedoPos - 1; i; --i)
            ++it;

        if (it != m_listUndo.end())
            SetBitmap(*it);
        Refresh();
    }
}

/******************************************************************************/
/*
 * Redo, if possible.
 */
void DocBase::Redo()
{
    std::list<BitmapBase*>::iterator it;
    int i;

    if (CanRedo())
    {
        // find the right position in the undo list
        it = m_listUndo.begin();
        for (i = m_nRedoPos; i; --i)
            ++it;

        if (it != m_listUndo.end())
            SetBitmap(*it);

        m_nRedoPos++;
        Refresh();
    }
}

/******************************************************************************/
/*
 * Return true if we can undo.
 */
bool DocBase::CanUndo()
{
    return m_nRedoPos > 1;
};

/******************************************************************************/
/*
 * Return true if we can redo.
 */
bool DocBase::CanRedo()
{
    return m_nRedoPos < m_listUndo.size();
};


/******************************************************************************/
/**
 * Load the file into a buffer and return it. If there is an error, show an
 * error message and return NULL and size 0.
 *
 * The caller must release the memory using delete[].
 */
uint8_t* DocBase::LoadToBuffer(size_t* pSize, const wxString& stringFilename)
{
    unsigned char* pBuff;
    wxFileOffset   len;
    wxFile         file(stringFilename);

    if (!file.IsOpened())
    {
        wxMessageBox(wxT("Could not open \"%s\" for reading."),
                stringFilename);
        return NULL;
    }

    len = file.Length();
    if (len > (wxFileOffset)(MC_MAX_FILE_BUFF_SIZE))
    {
        ::wxMessageBox(wxT("File format unknown."), wxT("Load Error"),
            wxOK | wxICON_ERROR);

        *pSize = 0;
        return NULL;
    }

    pBuff = new unsigned char[len];
    if (file.Read(pBuff, len) != len)
    {
        ::wxMessageBox(wxT("File could not be read, it may be broken."),
                       wxT("Load Error"),
            wxOK | wxICON_ERROR);
    }
    *pSize = (size_t) len;
    return pBuff;
}

/******************************************************************************/
/**
 * Do everything that has to be done after a file has been loaded or after
 * loading a file failed.
 *
 * in:
 *          stringFilename  file name
 *          bLoaded         true if the file was loaded successfully
 * return:
 *          bLoaded
 */
bool DocBase::PostLoad(const wxString& stringFilename, bool bLoaded)
{
    if (bLoaded)
    {
        m_listUndo.clear();
        m_nRedoPos = 0;
        PrepareUndo();

        m_fileName.Assign(stringFilename);
        Modify(false);

        Refresh();
    }
    else
    {
        ::wxMessageBox(wxT("Could not load this file."),
            wxT("Load Error"), wxOK | wxICON_ERROR);
    }
    return bLoaded;
}
