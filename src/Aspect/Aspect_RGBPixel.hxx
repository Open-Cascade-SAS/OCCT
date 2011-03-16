#ifndef _Aspect_RGBPixel_HeaderFile
#define _Aspect_RGBPixel_HeaderFile

#include <Standard_Type.hxx>
typedef struct {
  float red;
  float green;
  float blue;
} Aspect_RGBPixel;

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_RGBPixel);

#endif
