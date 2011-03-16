// File:	AIS_C0RegularityFilter.cxx
// Created:	Wed Feb  4 19:04:30 1998
// Author:	Julia GERASIMOVA
//		<jgv@orthodox.nnov.matra-dtv.fr>


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
