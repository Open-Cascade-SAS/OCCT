// File:        Aspect_RGBPixel.cxx
// Created:     28-OCT-98
// Author:      DCB
// Copyright:   MatraDatavision 1998

#include <Aspect_RGBPixel.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_RGBPixel)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type("Aspect_RGBPixel",sizeof(Aspect_RGBPixel));
  return _atype;
}
