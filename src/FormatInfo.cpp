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

#include "FormatInfo.h"


std::list<const FormatInfo*>* FormatInfo::m_pListFormatInfo;

FormatInfo::FormatInfo(
    const wxChar* pStrName,
    Filter* pFilters,
    DocBase* (*docFactory)() ) :
        m_stringName(pStrName),
        m_pFilters(pFilters),
        m_docFactory(docFactory)
{
    if (!m_pListFormatInfo)
    {
        m_pListFormatInfo = new std::list<const FormatInfo*>;
    }

    m_pListFormatInfo->push_back(this);
}


/*****************************************************************************/
/**
 * Create a filter string for wxFileDialog etc. This string contains:
 * - All image files (all filters)
 * - One entry for each type
 * - All files
 *
 */
wxString FormatInfo::GetFullFilterString()
{
    std::list<const FormatInfo*>::iterator i;
    wxString strTmp;
    wxString str;
    int k;

    // collect all filters
    for (i = (*m_pListFormatInfo).begin();
         i != (*m_pListFormatInfo).end();
         i++)
    {
        strTmp.append(i->GetFilterWildcards());
    }
#error hier weitermachen
#if 0
    str.append("All image files (");
    for (i =  (*m_pListFormatInfo).begin();
         i != (*m_pListFormatInfo).end();
         i++)
    {
#endif
}


/*****************************************************************************/
/**
 * Get all wildcards of this format info, e.g. ("*.koa;*.kla;*.ami").
 */
wxString FormatInfo::GetFilterWildcards() const
{
    int i;
    wxString str;

    for (i = 0; m_pFilters[i].pStrWildcard != NULL; i++)
    {
        if (i > 0)
        {
            str.append(wxT(";"));
        }

        str.append(m_pFilters[i].pStrWildcard);
    }

    return str;
}


/*****************************************************************************/
/**
 * Return a list containing information about all supported image formats.
 */
const std::list<const FormatInfo*>* FormatInfo::GetFormatList()
{
    return m_pListFormatInfo;
}
