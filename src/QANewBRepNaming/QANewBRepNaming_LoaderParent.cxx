// Created on: 1999-10-25
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999-1999 Matra Datavision
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


#include <BRepBuilderAPI_MakeShape.hxx>
#include <QANewBRepNaming_LoaderParent.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes Generator - Dangle.
//=======================================================================
Standard_Boolean QANewBRepNaming_LoaderParent::GetDangleShapes(const TopoDS_Shape& ShapeIn,
						    const TopAbs_ShapeEnum GeneratedFrom,
						    TopTools_DataMapOfShapeShape& Dangles) 
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Bind(ancestors.First(), mayBeDangle);
  }
  return Dangles.Extent();
}

//=======================================================================
//function : GetDangleShapes
//purpose  : Returns dangle sub shapes.
//=======================================================================
/*
Standard_Boolean QANewBRepNaming_LoaderParent::GetDangleShapes(const TopoDS_Shape& ShapeIn,
						    const TopAbs_ShapeEnum GeneratedFrom,
						    TopTools_MapOfShape& Dangles) 
{
  Dangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) Dangles.Add(mayBeDangle);
  }
  return Dangles.Extent();
}
*/
//=======================================================================
//function : LoadGeneratedDangleShapes
//purpose  : 
//=======================================================================

void QANewBRepNaming_LoaderParent::LoadGeneratedDangleShapes(const TopoDS_Shape&          ShapeIn,
						  const TopAbs_ShapeEnum       GeneratedFrom,
						  TNaming_Builder&             Builder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!QANewBRepNaming_LoaderParent::GetDangleShapes(ShapeIn, GeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next()) Builder.Generated(itr.Key(), itr.Value());
}

