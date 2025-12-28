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
#include <Standard_Type.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_C0RegularityFilter, SelectMgr_Filter)

//=================================================================================================

AIS_C0RegularityFilter::AIS_C0RegularityFilter(const TopoDS_Shape& aShape)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> SubShapes;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, SubShapes);
  bool Ok;
  for (int i = 1; i <= SubShapes.Extent(); i++)
  {
    Ok = false;
    NCollection_List<TopoDS_Shape>::Iterator it(SubShapes(i));
    TopoDS_Face                        Face1, Face2;
    if (it.More())
    {
      Face1 = TopoDS::Face(it.Value());
      it.Next();
      if (it.More())
      {
        Face2 = TopoDS::Face(it.Value());
        it.Next();
        if (!it.More())
        {
          GeomAbs_Shape ShapeContinuity =
            BRep_Tool::Continuity(TopoDS::Edge(SubShapes.FindKey(i)), Face1, Face2);
          Ok = (ShapeContinuity == GeomAbs_C0);
        }
      }
    }
    if (Ok)
    {
      const TopoDS_Shape& curEdge = SubShapes.FindKey(i);
      myMapOfEdges.Add(curEdge);
    }
  }
}

//=================================================================================================

bool AIS_C0RegularityFilter::ActsOn(const TopAbs_ShapeEnum aType) const
{
  return (aType == TopAbs_EDGE);
}

//=================================================================================================

bool AIS_C0RegularityFilter::IsOk(const occ::handle<SelectMgr_EntityOwner>& EO) const
{
  occ::handle<StdSelect_BRepOwner> aBO(occ::down_cast<StdSelect_BRepOwner>(EO));
  if (aBO.IsNull())
    return false;

  const TopoDS_Shape& aShape = aBO->Shape();

  if (aShape.ShapeType() != TopAbs_EDGE)
    return false;

  return (myMapOfEdges.Contains(aShape));
}
