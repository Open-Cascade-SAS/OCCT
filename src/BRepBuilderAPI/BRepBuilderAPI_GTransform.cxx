// Created on: 1996-12-30
// Created by: Stagiaire Mary FABIEN
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


#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepTools_GTrsfModification.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
#include <gp.hxx>
#include <gp_GTrsf.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

//=======================================================================
//function : BRepBuilderAPI_GTransform
//purpose  : 
//=======================================================================
BRepBuilderAPI_GTransform::BRepBuilderAPI_GTransform (const gp_GTrsf& T) :
  myGTrsf(T)
{
  myModification = new BRepTools_GTrsfModification(T);
}


//=======================================================================
//function : BRepBuilderAPI_GTransform
//purpose  : 
//=======================================================================

BRepBuilderAPI_GTransform::BRepBuilderAPI_GTransform (const TopoDS_Shape& S,
					const gp_GTrsf& T,
					const Standard_Boolean Copy) :
  myGTrsf(T)
{
  myModification = new BRepTools_GTrsfModification(T);
  Perform(S,Copy);
}



//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_GTransform::Perform(const TopoDS_Shape& S,
				 const Standard_Boolean Copy)
{
  BRepBuilderAPI_NurbsConvert nc;
  nc.Perform(S, Copy);
  myHist.Add(S,nc);
  TopoDS_Shape Slocal = nc.Shape();
  Handle(BRepTools_GTrsfModification) theModif =
    Handle(BRepTools_GTrsfModification)::DownCast(myModification);
  theModif->GTrsf() = myGTrsf;
  DoModif(Slocal,myModification);
//  myHist.Filter (Shape());
}


//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepBuilderAPI_GTransform::Modified
  (const TopoDS_Shape& F)
{
  myGenerated.Clear();
  const TopTools_DataMapOfShapeListOfShape& M = myHist.Modification();
  if (M.IsBound(F)) { 
    TopTools_ListOfShape Li;
    TopTools_ListIteratorOfListOfShape itL(M(F));
    for (;itL.More();itL.Next())
      Li.Assign(BRepBuilderAPI_ModifyShape::Modified(itL.Value()));
  }
  return myGenerated;
}


//=======================================================================
//function : ModifiedShape
//purpose  : 
//=======================================================================

TopoDS_Shape BRepBuilderAPI_GTransform::ModifiedShape
  (const TopoDS_Shape& S) const
{
  const TopTools_DataMapOfShapeListOfShape &aMapModif = myHist.Modification();
  TopoDS_Shape                              aShape    = S;

  if (aMapModif.IsBound(S)) {
    const TopTools_ListOfShape &aListModShape = aMapModif(S);
    Standard_Integer            aNbShapes     = aListModShape.Extent();

    if (aNbShapes > 0)
      aShape = aListModShape.First();
  }

  return BRepBuilderAPI_ModifyShape::ModifiedShape(aShape);
}

