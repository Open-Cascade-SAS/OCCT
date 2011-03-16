//============================================================================
//==== Titre: ImageUtility_X11Display.hxx
//==== Role : The header file of primitve type "X11Display" from package
//====  "AlienImage"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef Display ImageUtility_X11Display;
//============================================================================

#ifndef _ImageUtility_X11Display_HeaderFile
#define _ImageUtility_X11Display_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11Display);
//============================================================================

#include <X11/Xlib.h>

typedef Display *ImageUtility_X11Display ;

#endif
