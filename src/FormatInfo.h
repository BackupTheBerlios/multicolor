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

#ifndef FORMATINFO_H
#define FORMATINFO_H

#include <stdint.h>
#include <list>
#include <wx/string.h>
#include <wx/filename.h>

/*
 * -------------------             -------------------
 * | pStrName        | *         1 | FormatInfo      |
 * | pStrWildcard    | ---------<> |                 |
 * |                 | m_pFilters  |                 |
 * -------------------             -------------------
 *
 * e.g.                            e.g.
 * Koala files / *.koa;*.kla       Multi Color Bitmap
 * Amica files / *.ami
 */

/**
 * These values will be added to find out how good a file matches a certain
 * format. Higher values mean higher priority.
 */
#define MC_FORMAT_MAGIC_MATCH       20
#define MC_FORMAT_EXTENSION_MATCH   10
#define MC_FORMAT_SIZE_MATCH         5
#define MC_FORMAT_ADDR_MATCH         1

class MCDoc;
class DocBase;

class FormatInfo
{
public:
    typedef struct Filter_s
    {
        const wxChar* pStrName;
        const wxChar* pStrWildcard;
    } Filter;

    FormatInfo(const wxChar* pStrName,
               Filter* pFilters,
               DocBase* (*docFactory)(),
               int (*checkFile)(uint8_t* pBuff, unsigned len, const wxFileName& fileName));

    const wxString& GetName() const;
    wxString GetFilterWildcards() const;
    wxString GetFilters() const;

    static wxString GetFullFilterString();
    static const std::list<const FormatInfo*>* GetFormatList();
    static const FormatInfo* FindBestFormat(uint8_t* pBuff,
                                            unsigned len,
                                            const wxFileName& fileName);

    DocBase* Factory() const;
    int CheckFormat(uint8_t* pBuff, unsigned len, const wxFileName& fileName) const;

protected:
    wxString m_stringName;
    Filter* m_pFilters;
    DocBase* (*m_docFactory)();
    int (*m_checkFormat)(uint8_t* pBuff, unsigned len, const wxFileName& fileName);

private:
    static std::list<const FormatInfo*>* m_pListFormatInfo;

};


/*****************************************************************************/
/**
 * Create and return a document of this format using its Factor method.
 */
inline DocBase* FormatInfo::Factory() const
{
    return m_docFactory();
}


/*****************************************************************************/
/**
 * Check how good data matches our document format. Return a sum of
 * MC_FORMAT_*_MATCH.
 */
inline int FormatInfo::CheckFormat(uint8_t* pBuff, unsigned len,
                                 const wxFileName& fileName) const
{
    return m_checkFormat(pBuff, len, fileName);
}

#endif // FORMATINFO_H
