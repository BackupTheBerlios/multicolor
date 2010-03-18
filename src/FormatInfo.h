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

#include <wx/string.h>
#include <list>

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

    FormatInfo(
        const wxChar* pStrName,
        Filter* pFilters,
        DocBase* (*docFactory)() );
    const wxString& GetName() const;
    wxString GetFilterWildcards() const;
    wxString GetFilters() const;

    static wxString GetFullFilterString();
    static const std::list<const FormatInfo*>* GetFormatList();

protected:
    wxString m_stringName;
    Filter* m_pFilters;
    DocBase* (*m_docFactory)();

private:
    static std::list<const FormatInfo*>* m_pListFormatInfo;

};

#endif // FORMATINFO_H
