// Created on: 1993-01-20
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <TopoDS_Shape.hxx>
#include <TopTools.hxx>
#include <TopTools_ShapeSet.hxx>

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void  TopTools::Dump(const TopoDS_Shape& Sh, Standard_OStream& S)
{
  TopTools_ShapeSet SSet;
  SSet.Add(Sh);
  SSet.Dump(Sh,S);
  SSet.Dump(S);
}


void TopTools::Dummy(const Standard_Integer)
{
}
