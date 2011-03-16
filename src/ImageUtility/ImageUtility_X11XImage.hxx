//============================================================================
//==== Titre: ImageUtility_X11XImage.hxx
//==== Role : The header file of primitve type "X11XImage" from package
//====  "ImageUtility"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef XImage ImageUtility_X11XImage;
//============================================================================

#ifndef _ImageUtility_X11XImage_HeaderFile
#define _ImageUtility_X11XImage_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11XImage);
//============================================================================

#include <X11/Xlib.h>

typedef XImage *ImageUtility_X11XImage ;

#endif
