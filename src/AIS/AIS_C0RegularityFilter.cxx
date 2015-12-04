// Created on: 1998-02-04
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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


#include <AIS_C0RegularityFilter.hxx>
#include <BRep_Tool.hxx>
#include <GeomAbs_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_C0RegularityFilter,SelectMgr_Filter)

//=======================================================================
//function : AIS_C0RegularityFilter
//purpose  : 
//=======================================================================
AIS_C0RegularityFilter::AIS_C0RegularityFilter(const TopoDS_Shape& aShape)
{
  TopTools_IndexedDataMapOfShapeListOfShape SubShapes;
  TopExp::MapShapesAndAncestors(aShape,TopAbs_EDGE,TopAbs_FACE,SubShapes);
  Standard_Boolean Ok;
  for (Standard_Integer i = 1; i <= SubShapes.Extent(); i++) {
    Ok = Standard_False;
    TopTools_ListIteratorOfListOfShape it(SubShapes(i));
    TopoDS_Face Face1, Face2;
    if (it.More()) {
      Face1 = TopoDS::Face(it.Value());
      it.Next();
      if (it.More()) {
	Face2 = TopoDS::Face(it.Value());
	it.Next();
	if (!it.More()) {
	  GeomAbs_Shape ShapeContinuity =
	    BRep_Tool::Continuity(TopoDS::Edge(SubShapes.FindKey(i)),Face1,Face2);
	  Ok = (ShapeContinuity == GeomAbs_C0);
	}
      }
    }
    if (Ok) {
      TopoDS_Shape curEdge = SubShapes.FindKey( i );
      myMapOfEdges.Add(curEdge);
    }
  }
}

//=======================================================================
//function : ActsOn
//purpose  : 
//=======================================================================

Standard_Boolean AIS_C0RegularityFilter::ActsOn(const TopAbs_ShapeEnum aType) const
{
  return (aType == TopAbs_EDGE);
}

//=======================================================================
//function : IsOk
//purpose  : 
//=======================================================================

Standard_Boolean AIS_C0RegularityFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO) const
{
  Handle(StdSelect_BRepOwner) aBO (Handle(StdSelect_BRepOwner)::DownCast(EO));
  if (aBO.IsNull())
    return Standard_False;

  const TopoDS_Shape& aShape = aBO->Shape();

  if(aShape.ShapeType()!= TopAbs_EDGE)
    return Standard_False;

  return (myMapOfEdges.Contains(aShape));
}
