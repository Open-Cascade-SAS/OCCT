// Created on: 1998-03-04
// Created by: Julia Gerasimova
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

//		<g_design>

#include <AIS_BadEdgeFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_BadEdgeFilter,SelectMgr_Filter)

//=======================================================================
//function : AIS_BadEdgeFilter
//purpose  : 
//=======================================================================
AIS_BadEdgeFilter::AIS_BadEdgeFilter()
{
  myContour=0;
}

//=======================================================================
//function : ActsOn
//purpose  : 
//=======================================================================

Standard_Boolean AIS_BadEdgeFilter::ActsOn(const TopAbs_ShapeEnum aType) const
{
  return (aType == TopAbs_EDGE);
}

//=======================================================================
//function : IsOk
//purpose  : 
//=======================================================================

Standard_Boolean AIS_BadEdgeFilter::IsOk(const Handle(SelectMgr_EntityOwner)& EO) const
{
  if (myContour==0)
    return Standard_True;

  Handle(StdSelect_BRepOwner) aBO (Handle(StdSelect_BRepOwner)::DownCast(EO));
  if (aBO.IsNull())
    return Standard_True;

  const TopoDS_Shape& aShape = aBO->Shape();

  if (myBadEdges.IsBound(myContour)) {
    TopTools_ListIteratorOfListOfShape it(myBadEdges.Find(myContour));
    for (; it.More(); it.Next()) {
      if (it.Value().IsSame(aShape))
	return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : AddEdge
//purpose  : 
//=======================================================================

void AIS_BadEdgeFilter::AddEdge(const TopoDS_Edge& anEdge,
				const Standard_Integer Index)
{
  if (myBadEdges.IsBound(Index)) {
    myBadEdges.ChangeFind(Index).Append(anEdge); 
  }
  else {
    TopTools_ListOfShape LS;
    LS.Append(anEdge);
    myBadEdges.Bind(Index,LS);
  }
}

//=======================================================================
//function : RemoveEdges
//purpose  : 
//=======================================================================

void AIS_BadEdgeFilter::RemoveEdges(const Standard_Integer Index)
{
  myBadEdges.UnBind(Index);
}

//=======================================================================
//function : RemoveEdges
//purpose  : 
//=======================================================================

void AIS_BadEdgeFilter::SetContour(const Standard_Integer Index)
{
  myContour = Index;
}
