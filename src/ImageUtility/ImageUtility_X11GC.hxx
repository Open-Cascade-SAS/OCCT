//============================================================================
//==== Titre: ImageUtility_X11GC.hxx
//==== Role : The header file of primitve type "X11GC" from package
//====  "ImageUtility"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef GC ImageUtility_X11GC;
//============================================================================

#ifndef _ImageUtility_X11GC_HeaderFile
#define _ImageUtility_X11GC_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11GC);
//============================================================================

#include <X11/Xlib.h>

typedef GC ImageUtility_X11GC ;

#endif
