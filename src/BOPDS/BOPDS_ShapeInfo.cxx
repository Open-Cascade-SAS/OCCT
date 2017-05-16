// Created by: Peter KURNEV
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


#include <Bnd_Box.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <TopoDS_Shape.hxx>

#include <stdio.h>

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
  void BOPDS_ShapeInfo::Dump()const
{
  const TopAbs_ShapeEnum aTS = ShapeType();
  printf(" %s", TopAbs::ShapeTypeToString (aTS));
  printf(" {");
  for (BOPCol_ListIteratorOfListOfInteger aIt(mySubShapes); aIt.More(); aIt.Next()) {
    Standard_Integer n = aIt.Value();
    printf(" %d", n);
  }
  printf(" }");
}
