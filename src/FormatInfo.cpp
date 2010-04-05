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

FormatInfo::FormatInfo(const wxChar* pStrName,
                       Filter* pFilters,
                       DocBase* (*docFactory)(),
                       int (*checkFormat)(uint8_t* pBuff, unsigned len,
                                          const wxFileName& fileName)) :
        m_stringName(pStrName),
        m_pFilters(pFilters),
        m_docFactory(docFactory),
        m_checkFormat(checkFormat)
{
    if (!m_pListFormatInfo)
    {
        m_pListFormatInfo = new std::list<const FormatInfo*>;
    }

    m_pListFormatInfo->push_back(this);
}


/*****************************************************************************/
/**
 * Return the name of this format.
 */
const wxString& FormatInfo::GetName() const
{
    return m_stringName;
}


/*****************************************************************************/
/**
 * Create a part for a filter string for wxFileDialog etc.
 * This string contains:
 * - One entry for each filter type of this format
 */
wxString FormatInfo::GetFilters() const
{
    int i;
    wxString str;

    for (i = 0; m_pFilters[i].pStrWildcard != NULL; i++)
    {
        str.append(m_pFilters[i].pStrName);
        str.append(wxT(" ("));
        str.append(m_pFilters[i].pStrWildcard);
        str.append(wxT(")|"));
        str.append(m_pFilters[i].pStrWildcard);
        str.append(wxT("|"));
    }

    return str;
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
    int n;

    // collect all filters
    str = wxT("All image files (");
    strTmp.clear();
    for (i = (*m_pListFormatInfo).begin();
         i != (*m_pListFormatInfo).end(); i++)
    {
        strTmp.append((*i)->GetFilterWildcards());
        strTmp.append(wxT(";"));
    }
    // remove the last semicolon
    if (strTmp.Len())
        strTmp = strTmp.Left(strTmp.Len() - 1);
    str.append(strTmp);
    str.append(wxT(")|"));
    str.append(strTmp);
    str.append(wxT("|"));

    // one filter for each format
    for (i =  (*m_pListFormatInfo).begin();
         i != (*m_pListFormatInfo).end(); i++)
    {
        str.append((*i)->GetFilters());
    }

    // all files (the last one must be without "|" at the end
    str.append(wxT("All files (*)|*"));

    return str;
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


/*****************************************************************************/
/**
 * Find the format which matches the data best. Return the format or NULL if
 * none of them fits.
 */
const FormatInfo* FormatInfo::FindBestFormat(uint8_t* pBuff,
                                             unsigned len,
                                             const wxFileName& fileName)
{
    const FormatInfo*  pBestFormat;
    int          nMostPoints, nPoints;
    std::list<const FormatInfo*>::const_iterator i;

    pBestFormat = NULL;
    nMostPoints = 0;
    for (i = m_pListFormatInfo->begin(); i != m_pListFormatInfo->end(); ++i)
    {
        nPoints = (*i)->CheckFormat(pBuff, len, fileName);
        if (nPoints > nMostPoints)
        {
            nMostPoints = nPoints;
            pBestFormat = *i;
        }
    }

    return pBestFormat;
}
