// Created on: 1996-03-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRepFeat_Gluer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================
void BRepFeat_Gluer::Build()
{
  myGluer.Perform();
  if (myGluer.IsDone()) {
    Done();
    myShape = myGluer.ResultingShape();
  }
  else {
    NotDone();
  }
}



//=======================================================================
//function : isdeleted
//purpose  : 
//=======================================================================

Standard_Boolean BRepFeat_Gluer::IsDeleted
   (const TopoDS_Shape& F) 
{
  return (myGluer.DescendantFaces(TopoDS::Face(F)).IsEmpty());
}

//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepFeat_Gluer::Modified
   (const TopoDS_Shape& F)
{
  if (F.ShapeType() == TopAbs_FACE) {
    const TopTools_ListOfShape& LS = myGluer.DescendantFaces(TopoDS::Face(F));
    if (!LS.IsEmpty()) {
      if (!LS.First().IsSame(F))
	return myGluer.DescendantFaces(TopoDS::Face(F));
    }
  }
  static TopTools_ListOfShape LIM;
  return  LIM;
}

