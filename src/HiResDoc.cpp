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

#include <ios>
#include <iostream>
#include <wx/wx.h>
#include <wx/file.h>

#include "MCApp.h"
#include "HiResDoc.h"
#include "DocRenderer.h"

/******************************************************************************/
/**
 * This is a list of all Filters for this image format.
 */
static FormatInfo::Filter m_aFilters[] =
{
    { wxT("Image System Hires files"), wxT("*.ish") },
    { wxT("Hi-Eddi files"), wxT("*.hed") },
    { wxT("Doodle files"), wxT("*.dd;*.ddl") },
    { NULL, NULL }
};

/**
 * Information about this image format.
 */
FormatInfo HiResDoc::m_formatInfo(
    wxT("HiRes Bitmap"),
    m_aFilters,
    HiResDoc::Factory);


/******************************************************************************/
/**
 *
 */
HiResDoc::HiResDoc()
    : m_bitmap()
    , m_bitmapBackup()
    , m_listUndo()
    , m_nRedoPos(0)
{
    PrepareUndo();

    // PrepareUndo sets m_bModified, reset it
    m_bModified = false;
}


/******************************************************************************/
/**
 * Create an object of this class.
 */
DocBase* HiResDoc::Factory()
{
    return new HiResDoc;
}


/******************************************************************************/
/**
 * Copy the contents of the given bitmap to our one. The pointer must point to
 * an object which has actually the same type as our bitmap.
 */
void HiResDoc::SetBitmap(const BitmapBase* pB)
{
    m_bitmap = *(HiResBitmap*) pB;
}


/******************************************************************************/
/*
 */
bool HiResDoc::Load(const wxString& stringFilename)
{
    bool           bLoaded = false;
    unsigned char* pBuff;
    size_t         size;

    pBuff = DocBase::LoadToBuffer(&size, stringFilename);
    if (!pBuff)
        return false;
#if 0
    bLoaded = LoadKoala(pBuff, (unsigned)size);

    if (!bLoaded)
        bLoaded = LoadAmica(pBuff, (unsigned)size);
#endif

    delete[] pBuff;
    return PostLoad(stringFilename, bLoaded);
}


/******************************************************************************
 *
 * Save the file. If a name is given, use this one. Otherwise use the
 * current name.
 */
bool HiResDoc::Save(const wxString& stringFilename)
{
    bool           bRet = false;
#if 0
    unsigned char* pBuff;
    size_t         len;
    size_t         written;
    wxFile         file;
    wxFileName     fileNameTmp(m_fileName);

    if (stringFilename.Length())
    {
        fileNameTmp.Assign(stringFilename);
    }

    file.Open(fileNameTmp.GetFullPath(), wxFile::write);

    if (!file.IsOpened())
    {
        wxMessageBox(wxT("Could not open \"%s\" for writing."),
                fileNameTmp.GetFullPath());
        return false;
    }

    pBuff = new unsigned char[MC_MAX_FILE_BUFF_SIZE];

    if (fileNameTmp.GetExt().CmpNoCase(wxT("ami")) == 0)
        len = SaveAmica(pBuff);
    else
        len = SaveKoala(pBuff);

    if (len <= 0)
    {
        ::wxMessageBox(wxT("Could not save this file."),
            wxT("Save Error"), wxOK | wxICON_ERROR);
    }
    else
    {
        written = file.Write(pBuff, len);

        if (written == len)
        {
            // remember name only if it went well
            m_fileName = fileNameTmp;
            Modify(false);
            bRet = true;
        }
        else
        {
            ::wxMessageBox(wxT("An error occurred while saving."),
                    wxT("Save Error"), wxOK | wxICON_ERROR);
        }
    }

    delete[] pBuff;
#endif
	return bRet;
}
