// File:        AlienImage_BMPHeader.cxx
// Created:     Quebex 26 October 1998
// Author:      DCB
// Notes:       Most of code is taken from Xw_load_bmp_image.c file (ZOV)
//
// Copyright:   MatraDatavision 1998

#include <AlienImage_BMPHeader.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_BMPHeader)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("AlienImage_BMPHeader", sizeof (AlienImage_BMPHeader));
  return _atype;
}
 
