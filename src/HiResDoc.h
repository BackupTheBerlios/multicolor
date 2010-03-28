#ifndef HIRESDOC_H
#define HIRESDOC_H

#include <list>
#include <vector>

#include "HiResBitmap.h"
#include "DocBase.h"
#include "FormatInfo.h"

class DocRenderer;

class HiResDoc : public DocBase
{
public:
    HiResDoc();
    static DocBase* Factory();

    const BitmapBase* GetBitmap() const;
    virtual BitmapBase* GetBitmap();
    virtual void SetBitmap(const BitmapBase*);
    virtual void BackupBitmap();
    virtual void RestoreBitmap();

    bool Load(const wxString& stringFilename);
    bool Save(const wxString& stringFilename);

    std::vector<HiResBitmap> m_listUndo;
    unsigned       m_nRedoPos;

protected:
    static FormatInfo m_formatInfo;

    /// The bitmap under work
    HiResBitmap  m_bitmap;

    /// Backup which holds the original state when a tool is in use
    HiResBitmap  m_bitmapBackup;
};


/******************************************************************************/
/**
 * Return a pointer to our bitmap (const).
 */
inline const BitmapBase* HiResDoc::GetBitmap() const
{
    return &m_bitmap;
}


/******************************************************************************/
/**
 * Return a pointer to our bitmap.
 */
inline BitmapBase* HiResDoc::GetBitmap()
{
    return &m_bitmap;
}


/******************************************************************************/
/**
 * Create a temporary backup of the current document bitmap state.
 */
inline void HiResDoc::BackupBitmap()
{
    m_bitmapBackup = m_bitmap;
}


/******************************************************************************/
/**
 * Restore the current bitmap from the temporary backup.
 */
inline void HiResDoc::RestoreBitmap()
{
    m_bitmap = m_bitmapBackup;
}


#endif // HIRESDOC_H
