// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <inspector/ShapeView_Tools.hxx>
#include <inspector/ShapeView_ItemShape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

// =======================================================================
// function : IsPossibleToExplode
// purpose :
// =======================================================================
Standard_Boolean ShapeView_Tools::IsPossibleToExplode(
  const TopoDS_Shape&                 theShape,
  NCollection_List<TopAbs_ShapeEnum>& theExplodeTypes)
{
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();

  if (!theExplodeTypes.Contains(aShapeType))
    theExplodeTypes.Append(aShapeType);

  if (theExplodeTypes.Extent() == TopAbs_SHAPE + 1) // all types are collected, stop
    return Standard_True;

  TopoDS_Iterator aSubShapeIt(theShape);
  for (int aCurrentIndex = 0; aSubShapeIt.More(); aSubShapeIt.Next(), aCurrentIndex++)
  {
    if (IsPossibleToExplode(aSubShapeIt.Value(), theExplodeTypes))
      return Standard_True;
  }
  return Standard_False;
}
