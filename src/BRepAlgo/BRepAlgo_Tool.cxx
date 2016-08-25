// Created on: 1995-10-23
// Created by: Yves FRICAUD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgo_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>

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
        const TopTools_ListOfShape& aLF = Map(i);
        if (aLF.Extent() < 2) {
          const TopoDS_Edge& anEdge = TopoDS::Edge(Map.FindKey(i));
          if (anEdge.Orientation() == TopAbs_INTERNAL) {
            const TopoDS_Face& aFace = TopoDS::Face(aLF.First());
            if (aFace.Orientation() != TopAbs_INTERNAL) {
              continue;
            }
          }
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
      Standard_Integer NbSub = 0;
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
      if (NbSub == 0)
        {
#ifdef OCCT_DEBUG
        cout << "No subhape in shape!" << endl;
#endif
        SS = TopoDS_Shape();
        }
    }
    break;
  default:
    break;
  }
  return SS;
}
