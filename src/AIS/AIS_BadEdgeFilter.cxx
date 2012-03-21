// Created on: 1998-03-04
// Created by: Julia Gerasimova
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
