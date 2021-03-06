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

#ifndef TOOLBASE_H
#define TOOLBASE_H

// forward declarations
class DocBase;


typedef enum DrawingMode_e
{
    MCDrawingModeIgnore,
    MCDrawingModeForce,
    MCDrawingModeLeast,
    MCDrawingModeIndex0,
    MCDrawingModeIndex1,
    MCDrawingModeIndex2,
    MCDrawingModeIndex3,
}
MCDrawingMode;


class ToolBase
{
public:

    ToolBase();
    virtual ~ToolBase();

    // Set the primary and secondary color for the tool before starting it
    void SetColors(int nColorPrimary, int nColorSecondary);

    // Set the document we have to work with
    void SetDoc(DocBase* pDoc);

    void SetDrawingMode(MCDrawingMode drawingMode);

    // Return the ID for this tool
    virtual int GetToolId() = 0;

    // Start the tool at the given coordinates (i.e. Mouse button down)
    virtual void Start(int x, int y, bool bSecondaryFunction);

    // Mouse has been moved while the button was kept pressed
    virtual void Move(int x, int y);

    // Finish the tool at the given coordinates (i.e. Mouse button up)
    virtual void End(int x, int y);

protected:
    int m_nColorPrimary;
    int m_nColorSecondary;
    int m_nColorSelected;
    bool m_bSecondaryFunction;
    unsigned m_xStart;
    unsigned m_yStart;
    MCDrawingMode m_drawingMode;
    DocBase* m_pDoc;
};


/*****************************************************************************/
/*
 * Set the drawing mode to be used.
 */
inline void ToolBase::SetDrawingMode(MCDrawingMode drawingMode)
{
    m_drawingMode = drawingMode;
}


/*****************************************************************************/
/*
 * Set the document we have to work with.
 */
inline void ToolBase::SetDoc(DocBase* pDoc)
{
    m_pDoc = pDoc;
}

#endif /* TOOLBASE_H */
