// File:	AIS_BadEdgeFilter.cxx
// Created:	Wed Mar  4 12:01:13 1998
// Author:	Julia Gerasimova
//		<g_design>


#include <AIS_BadEdgeFilter.ixx>

#include <StdSelect_BRepOwner.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfIntegerListOfShape.hxx>



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
  if (Handle(StdSelect_BRepOwner)::DownCast(EO).IsNull())
    return Standard_True;

  if (myContour==0)
    return Standard_True;

  const TopoDS_Shape& aShape = ((Handle(StdSelect_BRepOwner)&)EO)->Shape();

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
