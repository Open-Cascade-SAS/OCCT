// File:	Aspect_IFStream.cxx
// Created:	Fri Jan 26 12:00:00 2001
// Author:	SPK


#include <Aspect_IFStream.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_IFStream)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type("Aspect_IFStream", sizeof(Aspect_IFStream));
  return _atype;
}
