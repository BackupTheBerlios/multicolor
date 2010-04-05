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
    HiResDoc::Factory,
    HiResDoc::CheckFormat);


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
 *
 */
int HiResDoc::CheckFormat(uint8_t* pBuff, unsigned len, const wxFileName& fileName)
{
    int match = 0;

    return match;
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
/**
 * Try to load the file from the given memory buffer. Return true for success.
 */
bool HiResDoc::Load(uint8_t* pBuff, unsigned size)
{
    bool bLoaded = false;
#if 0
    bLoaded = LoadKoala(pBuff, (unsigned)size);

    if (!bLoaded)
        bLoaded = LoadAmica(pBuff, (unsigned)size);
#endif
    return bLoaded;
}


/******************************************************************************
 **
 * Save the file to the memory buffer. Return the number of bytes used.
 * The file name is for informational purposes only, e.g. to decide
 * which sub-format to use.
 */
unsigned HiResDoc::Save(uint8_t* pBuff, const wxFileName& fileName)
{
    unsigned len = 0;
#if 0
    if (fileName.GetExt().CmpNoCase(wxT("ami")) == 0)
        len = SaveAmica(pBuff);
    else
        len = SaveKoala(pBuff);
#endif
    return len;
}
