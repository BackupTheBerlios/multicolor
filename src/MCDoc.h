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

#include <list>
#include <vector>

#include "MCBitmap.h"
#include "DocBase.h"
#include "FormatInfo.h"

#define MC_UNDO_LEN 100

class DocRenderer;

class MCDoc : public DocBase
{
public:
    MCDoc();
    virtual ~MCDoc();
    static DocBase* Factory();

    const BitmapBase* GetBitmap() const;
    virtual BitmapBase* GetBitmap();
    virtual void BackupBitmap();
    virtual void RestoreBitmap();

    virtual void Refresh(int x1 = 0, int y1 = 0, int x2 = 4096, int y2 = 4096);
    virtual void PrepareUndo();
    virtual void Undo();
    virtual void Redo();
    virtual bool CanUndo();
    virtual bool CanRedo();

    bool Load(const wxString& stringFilename);
    bool Save(const wxString& stringFilename);

    std::vector<MCBitmap> m_listUndo;
    unsigned       m_nRedoPos;

protected:
    static FormatInfo m_formatInfo;

    /// The bitmap under work
    MCBitmap  m_bitmap;

    /// Backup which holds the original state when a tool is in use
    MCBitmap  m_bitmapBackup;

    bool LoadKoala(unsigned char* pBuff, unsigned nSize);
    bool LoadAmica(unsigned char* pBuff, unsigned nSize);
    int SaveKoala(unsigned char* pBuff);
    int SaveAmica(unsigned char* pBuff);
    unsigned char* SaveAmicaFlush(
            unsigned char* pBuff, unsigned char nCount, unsigned char nVal);
};


/******************************************************************************/
/**
 * Return a pointer to our bitmap (const).
 */
inline const BitmapBase* MCDoc::GetBitmap() const
{
    return &m_bitmap;
}


/******************************************************************************/
/**
 * Return a pointer to our bitmap.
 */
inline BitmapBase* MCDoc::GetBitmap()
{
    return &m_bitmap;
}


/******************************************************************************/
/**
 * Create a temporary backup of the current document bitmap state.
 */
inline void MCDoc::BackupBitmap()
{
    m_bitmapBackup = m_bitmap;
}


/******************************************************************************/
/**
 * Restore the current bitmap from the temporary backup.
 */
inline void MCDoc::RestoreBitmap()
{
    m_bitmap = m_bitmapBackup;
}

#endif /* MCDOC_H */
