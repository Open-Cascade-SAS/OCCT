#ifndef _Image_HPrivateImage_HeaderFile
#define _Image_HPrivateImage_HeaderFile

#include <NCollection_Handle.hxx>

// This typedef shadows the private image storage class
// Currently FreeImagePlus is used

class fipImage;
typedef NCollection_Handle<fipImage> Image_HPrivateImage;

#endif /*_Image_HPrivateImage_HeaderFile*/
