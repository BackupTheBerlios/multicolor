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
#ifndef DOCBASE_H
#define DOCBASE_H

#include <stdint.h>
#include <list>
#include <wx/filename.h>
#include <wx/gdicmn.h>

#define MC_UNDO_LEN 100

class DocRenderer;
class BitmapBase;
class FormatInfo;

class DocBase
{
public:
    DocBase();
    virtual ~DocBase();

    void AddRenderer(DocRenderer* pRenderer);
    void RemoveRenderer(DocRenderer* pRenderer);
    void Refresh(int x1 = 0, int y1 = 0, int x2 = 4096, int y2 = 4096);

    bool IsModified();
    void Modify(bool bModified);

    const wxFileName& GetFileName() const;
    void SetFileName(const wxFileName& fileName);

    virtual const FormatInfo* GetFormatInfo() const = 0;

    virtual BitmapBase* GetBitmap() = 0;
    virtual void SetBitmap(const BitmapBase*) = 0;
    virtual void BackupBitmap() = 0;
    virtual void RestoreBitmap() = 0;

    void RefreshDirty();

    void PrepareUndo();
    void Undo();
    void Redo();
    bool CanUndo();
    bool CanRedo();
    void ClearUndoBuffer();

    static DocBase* Load(const wxString& stringFileName);
    bool Save(const wxString& stringFileName);

    void SetMousePos(int x, int y);
    const wxPoint& GetMousePos() const;

protected:
    virtual bool Load(uint8_t* pBuff, unsigned size) = 0;
    virtual unsigned Save(uint8_t* pBuff, const wxFileName& fileName) = 0;

    /// the full path and file name
    wxFileName                  m_fileName;

    static unsigned             m_nDocNumber;

    std::list<BitmapBase*>      m_listUndo;
    unsigned                    m_nRedoPos;

    /// true if the document has been changed but not saved
    bool                        m_bModified;

    /// last mouse position reported by one of my views (bitmap coordinates)
    wxPoint                     m_pointMousePos;

    /// A list of all Renderers for this document
    std::list<DocRenderer*>     m_listDocRenderers;

private:
    /// Copy construtor is private: This can't be copied
    DocBase(DocBase &r);
};

/******************************************************************************/
/**
 * Return true if the document has been modified but not saved.
 */
inline bool DocBase::IsModified()
{
    return m_bModified;
}


/******************************************************************************/
/**
 * Get a const reference to the file name.
 */
inline const wxFileName& DocBase::GetFileName() const
{
    return m_fileName;
}


/******************************************************************************/
/**
 * Set the file name.
 */
inline void DocBase::SetFileName(const wxFileName& fileName)
{
    m_fileName = fileName;
}


/******************************************************************************/
/**
 * Get the last mouse position reported by one of my views (bitmap coordinates)
 */
inline const wxPoint& DocBase::GetMousePos() const
{
    return m_pointMousePos;
}

#endif // DOCBASE_H
