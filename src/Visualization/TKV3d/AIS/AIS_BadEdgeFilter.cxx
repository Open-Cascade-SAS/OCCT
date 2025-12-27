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
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_BadEdgeFilter, SelectMgr_Filter)

//=================================================================================================

AIS_BadEdgeFilter::AIS_BadEdgeFilter()
{
  myContour = 0;
}

//=================================================================================================

bool AIS_BadEdgeFilter::ActsOn(const TopAbs_ShapeEnum aType) const
{
  return (aType == TopAbs_EDGE);
}

//=================================================================================================

bool AIS_BadEdgeFilter::IsOk(const occ::handle<SelectMgr_EntityOwner>& EO) const
{
  if (myContour == 0)
    return true;

  occ::handle<StdSelect_BRepOwner> aBO(occ::down_cast<StdSelect_BRepOwner>(EO));
  if (aBO.IsNull())
    return true;

  const TopoDS_Shape& aShape = aBO->Shape();

  if (myBadEdges.IsBound(myContour))
  {
    NCollection_List<TopoDS_Shape>::Iterator it(myBadEdges.Find(myContour));
    for (; it.More(); it.Next())
    {
      if (it.Value().IsSame(aShape))
        return false;
    }
  }
  return true;
}

//=================================================================================================

void AIS_BadEdgeFilter::AddEdge(const TopoDS_Edge& anEdge, const int Index)
{
  if (myBadEdges.IsBound(Index))
  {
    myBadEdges.ChangeFind(Index).Append(anEdge);
  }
  else
  {
    NCollection_List<TopoDS_Shape> LS;
    LS.Append(anEdge);
    myBadEdges.Bind(Index, LS);
  }
}

//=================================================================================================

void AIS_BadEdgeFilter::RemoveEdges(const int Index)
{
  myBadEdges.UnBind(Index);
}

//=================================================================================================

void AIS_BadEdgeFilter::SetContour(const int Index)
{
  myContour = Index;
}
