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

#define ISH_START_ADDR 0x4000
#define ISH_PADDING    192
#define IPH_START_ADDR 0x4000

/* File structure of an Image System HiRes image including start address */
typedef struct ish_s
{
   uint8_t ptr[2];            /* start address */
   uint8_t bitmap[8000];      /* 320 * 200 * 1 bit */
   uint8_t padding[192];      /* pad to 8192 bytes */
   uint8_t scr_ram[25 * 40];  /* both nibbles used */
} ish_t;

/* File structure of an Interpaint HiRes image including start address */
typedef struct iph_s
{
   uint8_t ptr[2];            /* start address */
   uint8_t bitmap[8000];      /* 320 * 200 * 1 bit */
   uint8_t scr_ram[25 * 40];  /* both nibbles used */
} iph_t;

/******************************************************************************/
/**
 * This is a list of all Filters for this image format.
 */
static FormatInfo::Filter m_aFilters[] =
{
    { wxT("Interpaint Hires files"), wxT("*.iph;*.ip64h") },
    { wxT("Image System Hires files"), wxT("*.ish") },
    { NULL, NULL }
};

/**
 * Information about this image format.
 */
FormatInfo HiResDoc::m_formatInfo(
    wxT("HiRes Bitmap"),
    wxT("iph"),
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
    uint16_t addr;
    int      match = 0;

    if (fileName.GetExt().CmpNoCase(wxT("iph")) == 0 ||
        fileName.GetExt().CmpNoCase(wxT("ip64h")) == 0 ||
        fileName.GetExt().CmpNoCase(wxT("ish")) == 0)
    {
        match += MC_FORMAT_EXTENSION_MATCH;
    }

    if (len == sizeof(ish_t) || len == sizeof(iph_t))
        match += MC_FORMAT_SIZE_MATCH;

    addr = pBuff[0] + pBuff[1] * 256;
    if (addr == ISH_START_ADDR ||
        addr == IPH_START_ADDR)
    {
        match += MC_FORMAT_ADDR_MATCH;
    }

    return match;
}


/******************************************************************************/
/**
 * Return a pointer to our FormatInfo.
 */
const FormatInfo* HiResDoc::GetFormatInfo() const
{
    return &m_formatInfo;
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
    bLoaded = LoadISH(pBuff, size);

    if (!bLoaded)
        bLoaded = LoadIPH(pBuff, size);

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

    if (fileName.GetExt().CmpNoCase(wxT("ish")) == 0)
        len = SaveISH(pBuff);
    else
        len = SaveIPH(pBuff);

    return len;
}



/*****************************************************************************/
/**
 * Try to load an Image System HiRes picture from the given buffer into this
 * document.
 * Return true if it worked and false otherwise.
 *
 * pBuff        Points to bytes from the file
 * nSize        Size
 * return       true if the file has been loaded
 */
bool HiResDoc::LoadISH(unsigned char* pBuff, unsigned nSize)
{
    ish_t*  pImage;
    int     i;

    if (nSize != sizeof(ish_t))
        return false;

    pImage = (ish_t*) pBuff;

    // ignore start addr, 2 bytes

    for (i = 0; i < 8000; ++i)
        m_bitmap.SetBitmapRAM(i, pImage->bitmap[i]);

    for (i = 0; i < 1000; ++i)
        m_bitmap.SetScreenRAM(i, pImage->scr_ram[i]);

    return true;
}


/*****************************************************************************/
/**
 * Try to load an Interpaint HiRes picture from the given buffer into this
 * document.
 * Return true if it worked and false otherwise.
 *
 * pBuff        Points to bytes from the file
 * nSize        Size
 * return       true if the file has been loaded
 */
bool HiResDoc::LoadIPH(unsigned char* pBuff, unsigned nSize)
{
    iph_t*  pImage;
    int     i;

    if (nSize != sizeof(iph_t))
        return false;

    pImage = (iph_t*) pBuff;

    // ignore start addr, 2 bytes

    for (i = 0; i < 8000; ++i)
        m_bitmap.SetBitmapRAM(i, pImage->bitmap[i]);

    for (i = 0; i < 1000; ++i)
        m_bitmap.SetScreenRAM(i, pImage->scr_ram[i]);

    return true;
}


/*****************************************************************************/
/**
 * Save a Image System Hires file into the given buffer.
 *
 * Return the number of bytes used in this buffer. The image includes the first
 * two bytes which form the load address of an PRG file.
 * If there's something wrong, return 0
 */
int HiResDoc::SaveISH(unsigned char* pBuff)
{
    unsigned char* p = pBuff;
    int i;

    // start addr
    *p++ = ISH_START_ADDR % 0x100;
    *p++ = ISH_START_ADDR / 0x100;

    for (i = 0; i < 8000; i++)
        *p++ = m_bitmap.GetBitmapRAM(i);

    for (i = 0; i < ISH_PADDING; i++)
        *p++ = 0;

    // screen
    for (i = 0; i < 1000; i++)
        *p++ = m_bitmap.GetScreenRAM(i);

    return p - pBuff;
}


/*****************************************************************************/
/**
 * Save a Interpaint Hires file into the given buffer.
 *
 * Return the number of bytes used in this buffer. The image includes the first
 * two bytes which form the load address of an PRG file.
 * If there's something wrong, return 0
 */
int HiResDoc::SaveIPH(unsigned char* pBuff)
{
    unsigned char* p = pBuff;
    int i;

    // start addr
    *p++ = IPH_START_ADDR % 0x100;
    *p++ = IPH_START_ADDR / 0x100;

    for (i = 0; i < 8000; i++)
        *p++ = m_bitmap.GetBitmapRAM(i);

    // screen
    for (i = 0; i < 1000; i++)
        *p++ = m_bitmap.GetScreenRAM(i);

    return p - pBuff;
}
