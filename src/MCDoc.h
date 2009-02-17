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
#ifndef MCDOC_H
#define MCDOC_H

#include <vector>
#include <wx/filename.h>

#include "MCBitmap.h"

#define MC_UNDO_LEN 100

class MCChildFrame;

class MCDoc
{
public:
    MCDoc();
    void SetFrame(MCChildFrame* pFrame);
    MCChildFrame* GetFrame();
    void Refresh();
    void PrepareUndo();
    void Undo();
    void Redo();
    bool CanUndo();
    bool CanRedo();
    //const wxPoint& GetMousePos();

    const wxFileName& GetFileName() const;
    bool Load(const wxString& stringFilename);
    bool Save(const wxString& stringFilename);

    MCBitmap  m_bitmap;

    // Die Original-Bitmap waehrend der Benutzung des Werkzeugs.
    MCBitmap  m_bitmapToolCopy;

    std::vector<MCBitmap> m_listUndo;
    unsigned       m_nRedoPos;

protected:
    static unsigned m_nDocNumber;

    // the full path and file name
    wxFileName m_fileName;

    // Points to the frame associated with this doc
    MCChildFrame* m_pFrame;

    bool LoadKoala(unsigned char* pBuff, unsigned nSize);
    bool LoadAmica(unsigned char* pBuff, unsigned nSize);
    int SaveKoala(unsigned char* pBuff);
    int SaveAmica(unsigned char* pBuff);
};

/******************************************************************************/
/*
 * Set the frame associated with this document.
 */
inline void MCDoc::SetFrame(MCChildFrame* pFrame)
{
    m_pFrame = pFrame;
}

/******************************************************************************/
/*
 * Set the frame associated with this document.
 */
inline MCChildFrame* MCDoc::GetFrame()
{
    return m_pFrame;
}

/******************************************************************************/
/*
 * Get a const reference to out file name.
 */
inline const wxFileName& MCDoc::GetFileName() const
{
    return m_fileName;
}

#endif /* MCDOC_H */
