//============================================================================
//==== Titre: ImageUtility_X11Window.hxx
//==== Role : The header file of primitve type "X11Window" from package
//====  "ImageUtility"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef Window ImageUtility_X11Window;
//============================================================================

#ifndef _ImageUtility_X11Window_HeaderFile
#define _ImageUtility_X11Window_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11Window);
//============================================================================

#include <X11/Xlib.h>

typedef Window ImageUtility_X11Window ;

#endif
