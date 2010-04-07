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
    static int CheckFormat(uint8_t* pBuff, unsigned len, const wxFileName& fileName);

    virtual const FormatInfo* GetFormatInfo() const;

    const BitmapBase* GetBitmap() const;
    virtual BitmapBase* GetBitmap();
    virtual void SetBitmap(const BitmapBase*);
    virtual void BackupBitmap();
    virtual void RestoreBitmap();

    std::vector<HiResBitmap> m_listUndo;
    unsigned       m_nRedoPos;

protected:
    virtual bool Load(uint8_t* pBuff, unsigned size);
    virtual unsigned Save(uint8_t* pBuff, const wxFileName& fileName);

    bool LoadISH(unsigned char* pBuff, unsigned nSize);
    bool LoadIPH(unsigned char* pBuff, unsigned nSize);
    int SaveISH(unsigned char* pBuff);
    int SaveIPH(unsigned char* pBuff);

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
