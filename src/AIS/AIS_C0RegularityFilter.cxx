// Created on: 1998-02-04
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <AIS_C0RegularityFilter.ixx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopExp.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <GeomAbs_Shape.hxx>
#include <BRep_Tool.hxx>
#include <StdSelect_BRepOwner.hxx>


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
  if (Handle(StdSelect_BRepOwner)::DownCast(EO).IsNull())
    return Standard_False;

  const TopoDS_Shape& aShape = ((Handle(StdSelect_BRepOwner)&)EO)->Shape();

  if(aShape.ShapeType()!= TopAbs_EDGE)
    return Standard_False;

  return (myMapOfEdges.Contains(aShape));
}
