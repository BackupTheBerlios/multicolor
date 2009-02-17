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
#include "MCChildFrame.h"

#define AMICA_SIG_BYTE 0xc2
#define AMICA_COL_ROT_TAB_SIZE 0
#define MC_MAX_FILE_BUFF_SIZE 0x10000

/* File structure of a Koala image including start address */
typedef struct KOALA_S
{
   unsigned char ptr[2]; /* start address */
   unsigned char bitmap[8000]; /* 160 * 200 * 2 bit*/
   unsigned char scr_ram[25 * 40];      /* both nibbles used */
   unsigned char col_ram[25 * 40];      /* low-nibble only */
   unsigned char background;
} KOALA_T;

// This contains a number for unnamed documents
unsigned MCDoc::m_nDocNumber;


/******************************************************************************
 */
MCDoc::MCDoc()
    : m_bitmap()
    , m_bitmapToolCopy()
    , m_listUndo()
    , m_nRedoPos(0)
    , m_fileName()
    , m_pFrame(NULL)
{
    PrepareUndo();
    //SetDocumentName(wxString::Format(_T("unnamed%d"), ++m_nDocNumber));
}

/******************************************************************************/
/*
 * Refresh the frame associated with this document.
 */
void MCDoc::Refresh()
{
    m_pFrame->Refresh(false);
}

/******************************************************************************
 */
bool MCDoc::Load(const wxString& stringFilename)
{
    bool           bLoaded = false;
    unsigned char* pBuff;
    wxFileOffset   pos;
    wxFile         file(stringFilename);

    if (!file.IsOpened())
    {
        wxMessageBox(wxT("Could not open \"%s\" for reading."),
                stringFilename);
        return false;
    }

    pos = file.Length();
    if (pos > (wxFileOffset)(MC_MAX_FILE_BUFF_SIZE))
    {
        ::wxMessageBox(wxT("File format unknown."), wxT("Load Error"),
            wxOK | wxICON_ERROR);
    }
    else
    {
        pBuff = new unsigned char[pos];
        file.Read(pBuff, pos);
        bLoaded = LoadKoala(pBuff, (unsigned)pos);

        if (!bLoaded)
            bLoaded = LoadAmica(pBuff, (unsigned)pos);

        delete[] pBuff;
    };

    if (!bLoaded)
    {
        ::wxMessageBox(wxT("Could not load this file."),
            wxT("Load Error"), wxOK | wxICON_ERROR);
    }

    m_listUndo.clear();
    m_nRedoPos = 0;
    PrepareUndo();

    m_fileName.Assign(stringFilename);
    m_pFrame->SetTitle(m_fileName.GetName());
    return true;
}


/******************************************************************************
 *
 * Save the file. If a name is given, use this one. Otherwise use the
 * current name.
 */
bool MCDoc::Save(const wxString& stringFilename)
{
    unsigned char* pBuff;
    size_t         len;
    size_t         written;
    wxFile         file;
    bool           bRet = false;
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
            m_pFrame->SetTitle(m_fileName.GetName());
            bRet = true;
        }
        else
        {
            ::wxMessageBox(wxT("An error occurred while saving."),
                    wxT("Save Error"), wxOK | wxICON_ERROR);
        }
    }

    delete[] pBuff;
	return bRet;
}

/******************************************************************************
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
    KOALA_T* pKoala;
    int      i;

    if (nSize != sizeof(KOALA_T))
        return false;

    pKoala = (KOALA_T*) pBuff;

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
    KOALA_T        koala;
    unsigned       nRead, nCount, i;
    unsigned char  nByte;
    unsigned char* p;
    unsigned char* pEnd;

    // could be a union...
    p    = (unsigned char*) &koala;
    pEnd = p + sizeof(KOALA_T);

    // ignore start addr, read next byte at offset 2
    nRead  = 2;
    p     += 2;

    for(;;)
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
    *p++ = 0x00;
    *p++ = 0x44;

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
    int nPos, i;
    int nVal, nCount, nNext;
    unsigned char* p;
    unsigned char* pSource;

    // make it easy for me and create a koala buffer first
    pSource = new unsigned char[MC_MAX_FILE_BUFF_SIZE];
    if (SaveKoala(pSource) != sizeof(KOALA_T))
    {
        delete[] pSource;
        return 0;
    }
    // add 256 bytes color rotation tab behind it, all black
    memset(pSource + sizeof(KOALA_T), 0, AMICA_COL_ROT_TAB_SIZE);

    // save start address
    p = pBuff;
    *p++ = 0x00;
    *p++ = 0x40;

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
            if ((nCount > 3) || (nVal == AMICA_SIG_BYTE))
            {
                // write: SIG count val
                *p++ = AMICA_SIG_BYTE;
                *p++ = nCount;
                *p++ = nVal;
            }
            else
            {
                // write literal data
                for (i = 0; i < nCount; ++i)
                    *p++ = nVal;
            }
            nVal = nNext;
            nCount = 1;
        }
        else
        {
            ++nCount;
        }
    }
    while (nPos < (sizeof(KOALA_T) + AMICA_COL_ROT_TAB_SIZE));

    // end mark
    *p++ = AMICA_SIG_BYTE;
    *p++ = 0;

    delete[] pSource;
    return p - pBuff;
}

/******************************************************************************
 * Return true if we can undo.
 */
bool MCDoc::CanUndo()
{
    return m_nRedoPos > 1;
};

/******************************************************************************
 * Return true if we can redo.
 */
bool MCDoc::CanRedo()
{
    return m_nRedoPos < m_listUndo.size();
};

/******************************************************************************/
/*
 * Save the current bitmap as undo step.
 */
void MCDoc::PrepareUndo()
{
    unsigned i;

    // if we are not at the end of the undo list, discard the rest
    while (m_nRedoPos < m_listUndo.size())
        m_listUndo.pop_back();

    // if the list is full, remove the first entry
    if (m_listUndo.size() >= MC_UNDO_LEN)
    {
        for (i = 1; i < m_listUndo.size(); ++i)
        {
            m_listUndo[i - 1] = m_listUndo[i];
        }
        m_listUndo.pop_back();
        --m_nRedoPos;
    }

    // append current state
    m_listUndo.push_back(m_bitmap);
    m_nRedoPos++;

}

/******************************************************************************/
/*
 * Undo, if possible.
 */
void MCDoc::Undo()
{
   if (CanUndo())
   {
      m_nRedoPos--;
      m_bitmap = m_listUndo[m_nRedoPos - 1];
   }
   m_pFrame->Refresh(false);
}

/******************************************************************************/
/*
 * Redo, if possible.
 */
void MCDoc::Redo()
{
   if (CanRedo())
   {
      m_bitmap = m_listUndo[m_nRedoPos];
      m_nRedoPos++;
   }
   m_pFrame->Refresh(false);
}
