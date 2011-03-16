//============================================================================
//==== Titre: Image_PixelAddress.hxx
//====
//==== Implementation:  This is a primitive type implemented with typedef
//============================================================================

#ifndef _Image_PixelAddress_HeaderFile
#define _Image_PixelAddress_HeaderFile

//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(Image_PixelAddress);
//============================================================================

#include <Aspect_Pixel.hxx>

typedef Aspect_Pixel *Image_PixelAddress ;

#endif
