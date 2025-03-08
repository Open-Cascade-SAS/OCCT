// Created on: 1994-12-09
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepBuilderAPI_Transform::BRepBuilderAPI_Transform(const gp_Trsf& T)
    : myTrsf(T)
{
  myModification = new BRepTools_TrsfModification(T);
}

//=================================================================================================

BRepBuilderAPI_Transform::BRepBuilderAPI_Transform(const TopoDS_Shape&    theShape,
                                                   const gp_Trsf&         theTrsf,
                                                   const Standard_Boolean theCopyGeom,
                                                   const Standard_Boolean theCopyMesh)
    : myTrsf(theTrsf)
{
  myModification = new BRepTools_TrsfModification(theTrsf);
  Perform(theShape, theCopyGeom, theCopyMesh);
}

//=================================================================================================

void BRepBuilderAPI_Transform::Perform(const TopoDS_Shape&    theShape,
                                       const Standard_Boolean theCopyGeom,
                                       const Standard_Boolean theCopyMesh)
{
  myUseModif = theCopyGeom || myTrsf.IsNegative()
               || (Abs(Abs(myTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec());
  if (myUseModif)
  {
    Handle(BRepTools_TrsfModification) theModif =
      Handle(BRepTools_TrsfModification)::DownCast(myModification);
    theModif->Trsf()       = myTrsf;
    theModif->IsCopyMesh() = theCopyMesh;
    DoModif(theShape, myModification);
  }
  else
  {
    myLocation = myTrsf;
    myShape    = theShape.Moved(myLocation);
    Done();
  }
}

//=================================================================================================

TopoDS_Shape BRepBuilderAPI_Transform::ModifiedShape(const TopoDS_Shape& S) const
{
  if (myUseModif)
  {
    return myModifier.ModifiedShape(S);
  }
  return S.Moved(myLocation);
}

//=================================================================================================

const TopTools_ListOfShape& BRepBuilderAPI_Transform::Modified(const TopoDS_Shape& F)
{
  if (!myUseModif)
  {
    myGenerated.Clear();
    myGenerated.Append(F.Moved(myLocation));
    return myGenerated;
  }
  return BRepBuilderAPI_ModifyShape::Modified(F);
}
