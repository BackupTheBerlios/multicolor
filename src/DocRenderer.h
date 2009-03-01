/*
 * MultiColor - An image manipulation tool for Commodore 8-bit computers'
 *              graphic formats
 *
 * (c) Thomas Giesel
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

#ifndef DOCRENDERER_H_
#define DOCRENDERER_H_

class MCDoc;

/*****************************************************************************/
/*
 * This abstract class defines an interface for Classes which want to be
 * notified if a document has changed.
 *
 * Objects implementing this type can register themselfes to get updates
 * when the document view has to be updated.
 */
class DocRenderer
{
public:
    /*
     * This is called when the document contents has changed, the parameters
     * report the area to be updated. Coordinates are in bitmap space.
     */
    virtual void OnDocChanged(
            int x1, int y1, int x2, int y2) = 0;

    /*
     * This is called when the mouse has been moved in one of the views.
     * Coordinates are in bitmap space.
     */
    virtual void OnDocMouseMoved(int x, int y) = 0;

    /*
     * This is called when a document is destroyed which is rendered by me
     */
    virtual void OnDocDestroy(MCDoc* pDoc) = 0;
};

#endif /*DOCRENDERER_H_*/
