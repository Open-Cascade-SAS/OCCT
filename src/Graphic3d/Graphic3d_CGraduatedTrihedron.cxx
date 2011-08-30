// File:      Graphic3d_CGraduatedTrihedron.cxx
// Created:   6 March 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE SA 2011

#include <Graphic3d_CGraduatedTrihedron.hxx>

const Handle(Standard_Type)& TYPE(Graphic3d_CGraduatedTrihedron)
{
  static Handle(Standard_Type) _atype = 
    new Standard_Type ("Graphic3d_CGraduatedTrihedron", sizeof (Graphic3d_CGraduatedTrihedron));
  return _atype;
}
