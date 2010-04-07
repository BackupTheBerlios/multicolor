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
#include "MCDoc.h"
#include "DocRenderer.h"

#define AMICA_SIG_BYTE 0xc2

/* File structure of a Koala image including start address */
typedef struct koala_s
{
   unsigned char ptr[2]; /* start address */
   unsigned char bitmap[8000]; /* 160 * 200 * 2 bit*/
   unsigned char scr_ram[25 * 40];      /* both nibbles used */
   unsigned char col_ram[25 * 40];      /* low-nibble only */
   unsigned char background;
} koala_t;

#define AMICA_START_ADDR 0x4000
#define KOALA_START_ADDR 0x6000

/******************************************************************************/
/**
 * This is a list of all Filters for this image format.
 */
static FormatInfo::Filter m_aFilters[] =
{
    { wxT("Koala files"), wxT("*.koa;*.kla") },
    { wxT("Amica files"), wxT("*.ami") },
    { NULL, NULL }
};

/**
 * Information about this image format.
 */
FormatInfo MCDoc::m_formatInfo(
    wxT("Multi Color Bitmap"),
    wxT("koa"),
    m_aFilters,
    MCDoc::Factory,
    MCDoc::CheckFormat);


/******************************************************************************/
/**
 *
 */
MCDoc::MCDoc()
    : m_bitmap()
    , m_bitmapBackup()
{
    PrepareUndo();

    // PrepareUndo sets m_bModified, reset it
    m_bModified = false;

    if (sizeof(koala_t) != 10003)
    {
        wxMessageBox(wxT("Warning: Koala structure has wrong size. This is not good at all."));
    }
}


/******************************************************************************/
/**
 * Create an object of this class.
 */
DocBase* MCDoc::Factory()
{
    return new MCDoc;
}


/******************************************************************************/
/**
 * Check how good data matches our document format. Return a sum of
 * MC_FORMAT_*_MATCH.
 */
int MCDoc::CheckFormat(uint8_t* pBuff, unsigned len, const wxFileName& fileName)
{
    uint16_t addr;
    int      match = 0;

    if (fileName.GetExt().CmpNoCase(wxT("ami")) == 0 ||
        fileName.GetExt().CmpNoCase(wxT("kla")) == 0 ||
        fileName.GetExt().CmpNoCase(wxT("koa")) == 0)
    {
        match += MC_FORMAT_EXTENSION_MATCH;
    }

    if (len == sizeof(koala_t))
        match += MC_FORMAT_SIZE_MATCH;

    addr = pBuff[0] + pBuff[1] * 256;
    if (addr == KOALA_START_ADDR ||
        addr == AMICA_START_ADDR)
    {
        match += MC_FORMAT_ADDR_MATCH;
    }

    return match;
}


/******************************************************************************/
/**
 * Return a pointer to our FormatInfo.
 */
const FormatInfo* MCDoc::GetFormatInfo() const
{
    return &m_formatInfo;
}


/******************************************************************************/
/**
 * Copy the contents of the given bitmap to our one. The pointer must point to
 * an object which has actually the same type as our bitmap.
 */
void MCDoc::SetBitmap(const BitmapBase* pB)
{
    m_bitmap = *(MCBitmap*) pB;
}


/******************************************************************************/
/**
 * Try to load the file from the given memory buffer. Return true for success.
 */
bool MCDoc::Load(uint8_t* pBuff, unsigned size)
{
    bool bLoaded = false;

    bLoaded = LoadKoala(pBuff, size);

    if (!bLoaded)
        bLoaded = LoadAmica(pBuff, size);

    return bLoaded;
}


/******************************************************************************
 **
 * Save the file to the memory buffer. Return the number of bytes used.
 * The file name is for informational purposes only, e.g. to decide
 * which sub-format to use.
 */
unsigned MCDoc::Save(uint8_t* pBuff, const wxFileName& fileName)
{
    unsigned len = 0;

    if (fileName.GetExt().CmpNoCase(wxT("ami")) == 0)
        len = SaveAmica(pBuff);
    else
        len = SaveKoala(pBuff);

    return len;
}


/*****************************************************************************/
/**
 * Try to load a Koala picture from the given buffer into this document.
 * Return true if it worked and false otherwise. False could also mean that
 * it isn't Koala.
 *
 * pBuff        Points to bytes from the file
 * nSize        Size
 * return       true if the file has been loaded
 */
bool MCDoc::LoadKoala(unsigned char* pBuff, unsigned nSize)
{
    koala_t* pKoala;
    int      i;

    if (nSize != sizeof(koala_t))
        return false;

    pKoala = (koala_t*) pBuff;

    // ignore start addr, 2 bytes

    // bitmap
    for (i = 0; i < 8000; ++i)
        m_bitmap.SetBitmapRAM(i, pKoala->bitmap[i]);
    // screen
    for (i = 0; i < 1000; ++i)
        m_bitmap.SetScreenRAM(i, pKoala->scr_ram[i]);
    // color
    for (i = 0; i < 1000; ++i)
        m_bitmap.SetColorRAM(i, pKoala->col_ram[i]);
    // background
    m_bitmap.SetBackground(C64Color(pKoala->background));

    return true;
}

/******************************************************************************
 * Try to load an Amica picture from the given buffer into this document.
 * Return true if it worked and false otherwise. False could also mean that
 * it isn't Amica.
 *
 * pBuff        Points to bytes from the file
 * nSize        Size
 * return       true if the file has been loaded
 */
bool MCDoc::LoadAmica(unsigned char* pBuff, unsigned nSize)
{
    koala_t        koala;
    unsigned       nRead, nCount, i;
    unsigned char  nByte;
    unsigned char* p;
    unsigned char* pEnd;

    // could be a union...
    p    = (unsigned char*) &koala;
    pEnd = p + sizeof(koala_t);

    // ignore start addr, read next byte at offset 2
    nRead  = 2;
    p     += 2;

    for (;;)
    {
        if (nRead >= nSize)
            return false; // EOF

        nByte = pBuff[nRead++];

        if (nByte == AMICA_SIG_BYTE)
        {
            if (nRead >= nSize)
                return false; // EOF

            nCount = pBuff[nRead++];

            /* unpack */
            if (nCount)
            {
                if (nRead >= nSize)
                    return false; // EOF

                nByte = pBuff[nRead++];

                for (i = 0; i < nCount; ++i)
                {
                    if (p >= pEnd)
                    {
                        /* no need to read the color rotation tab */
                        break;
                    }

                    *p++ = nByte;
                }
            }
            else
            {
                /* normal end of file */
                break;
            }
        }
        else
        {
            *p++ = nByte;
            /* no need to read the color rotation tab */
            if (p >= pEnd)
                break;
        }
    }

    if (p < pEnd)
    {
        wxMessageBox(wxT("Warning: File too short or damaged"));
    }

    for (i = 0; i < 8000; ++i)
        m_bitmap.SetBitmapRAM(i, koala.bitmap[i]);
    for (i = 0; i < 1000; ++i)
        m_bitmap.SetScreenRAM(i, koala.scr_ram[i]);
    for (i = 0; i < 1000; ++i)
        m_bitmap.SetColorRAM(i, koala.col_ram[i]);
    m_bitmap.SetBackground(C64Color(koala.background));

    return true;
}

/******************************************************************************
 * Save a koala file image into the given buffer.
 *
 * Return the number of bytes used in this buffer. The image includes the first
 * two bytes which form the load address of an PRG file.
 * If there's something wrong, return 0
 */
int MCDoc::SaveKoala(unsigned char* pBuff)
{
    unsigned char* p = pBuff;
    int i;

    // start addr
    *p++ = KOALA_START_ADDR % 0x100;
    *p++ = KOALA_START_ADDR / 0x100;

    // bitmap
    for (i = 0; i < 8000; i++)
        *p++ = m_bitmap.GetBitmapRAM(i);

    // screen
    for (i = 0; i < 1000; i++)
        *p++ = m_bitmap.GetScreenRAM(i);

    // color
    for (i = 0; i < 1000; i++)
        *p++ = m_bitmap.GetColorRAM(i);

    // background
    *p++ = m_bitmap.GetBackground();

    return p - pBuff;
}

/******************************************************************************
 * Save a Amica file image into the given buffer.
 *
 * Return the number of bytes used in this buffer. The image includes the first
 * two bytes which form the load address of an PRG file.
 * If there's something wrong, return 0
 */
int MCDoc::SaveAmica(unsigned char* pBuff)
{
    unsigned       nPos;
    unsigned       nVal, nCount, nNext;
    unsigned char* p;
    unsigned char* pSource;

    // make it easy for me and create a koala buffer first
    pSource = new unsigned char[MC_MAX_FILE_BUFF_SIZE];
    if (SaveKoala(pSource) != sizeof(koala_t))
    {
        delete[] pSource;
        return 0;
    }

    // save start address
    p = pBuff;
    *p++ = AMICA_START_ADDR % 0x100;
    *p++ = AMICA_START_ADDR / 0x100;

    // start reading behind the loading address of the koala buffer
    nPos = 2;

    // get the first byte
    nVal = pSource[nPos++];
    nCount = 1;
    do
    {
        nNext = pSource[nPos++];

        // is there a reason to write now?
        if ((nVal != nNext) || (nCount == 255))
        {
            // write compressed data if we have enough
            // or if we have the signal byte
            p = SaveAmicaFlush(p, nCount, nVal);
            nVal = nNext;
            nCount = 1;
        }
        else
        {
            ++nCount;
        }
    }
    while (nPos < (sizeof(koala_t)));

    // save remaining bytes, if there are any
    if (nCount)
        p = SaveAmicaFlush(p, nCount, nVal);

    // end mark
    *p++ = AMICA_SIG_BYTE;
    *p++ = 0;

    delete[] pSource;
    return p - pBuff;
}


/******************************************************************************
 * Flush collected bytes to an RLE Amica sequence.
 *
 * Return the advanced target buffer pointer.
 */
unsigned char* MCDoc::SaveAmicaFlush(
        unsigned char* pBuff, unsigned char nCount, unsigned char nVal)
{
    unsigned i;

    if ((nCount > 3) || (nVal == AMICA_SIG_BYTE))
    {
        // write: SIG count val
        *pBuff++ = AMICA_SIG_BYTE;
        *pBuff++ = nCount;
        *pBuff++ = nVal;
    }
    else
    {
        // write literal data
        for (i = 0; i < nCount; ++i)
            *pBuff++ = nVal;
    }

    return pBuff;
}

