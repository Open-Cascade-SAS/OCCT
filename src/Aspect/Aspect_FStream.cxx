// File:	Aspect_FStream.cxx
// Created:	Wed Aug 24 09:26:14 1994
// Author:	Modelistation
//		<model@bravox>


#include <Aspect_FStream.hxx>

const Handle(Standard_Type)& STANDARD_TYPE(Aspect_FStream)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type("Aspect_FStream",sizeof(Aspect_FStream));
  return _atype;
}
