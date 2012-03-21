// Created on: 1995-10-23
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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



#include <BRepAlgo_Tool.ixx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopExp.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Edge.hxx>




//=======================================================================
//function : Deboucle3D
//purpose  : 
//=======================================================================

TopoDS_Shape BRepAlgo_Tool::Deboucle3D(const TopoDS_Shape& S,
					 const TopTools_MapOfShape& Boundary)
{
  TopoDS_Shape SS;

  switch ( S.ShapeType()) {
  case TopAbs_FACE:
    {
    }
    break;
  case TopAbs_SHELL: 
    {
      // if the shell contains free borders that do not belong to the 
      // free borders of caps ( Boundary) it is removed.
      TopTools_IndexedDataMapOfShapeListOfShape Map;
      TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,Map);
      
      Standard_Boolean JeGarde = Standard_True;
      for ( Standard_Integer i = 1; i <= Map.Extent() && JeGarde; i++) {
	if (Map(i).Extent() < 2) {
	  const TopoDS_Edge& anEdge = TopoDS::Edge(Map.FindKey(i));
	  if (!Boundary.Contains(anEdge)  && 
	      !BRep_Tool::Degenerated(anEdge) )
	    JeGarde = Standard_False;
	}
      }
      if ( JeGarde) SS = S;
    }
    break;
  case TopAbs_COMPOUND:  
  case TopAbs_SOLID:
    {
      // iterate on sub-shapes and add non-empty.
      TopoDS_Iterator it(S);
      TopoDS_Shape SubShape;
      Standard_Boolean NbSub = 0;
      BRep_Builder B;
      if (S.ShapeType() == TopAbs_COMPOUND) {
	B.MakeCompound(TopoDS::Compound(SS));
      }
      else {
	B.MakeSolid(TopoDS::Solid(SS));
      }
      for ( ; it.More(); it.Next()) {
	const TopoDS_Shape& CurS = it.Value();
	SubShape = Deboucle3D(CurS,Boundary);
	if ( !SubShape.IsNull()) {
	  B.Add(SS, SubShape);
	  NbSub++;
	}
      }
      if (NbSub == 0) SS = TopoDS_Shape();
    }
    break;
  default:
    break;
  }
  return SS;
}
