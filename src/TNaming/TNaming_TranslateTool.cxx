// Created on: 1999-06-24
// Created by: Sergey ZARITCHNY
// Copyright (c) 1999-1999 Matra Datavision
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
#include <BRep_Curve3D.hxx>
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveOnSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListOfPointRepresentation.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_PolygonOnClosedSurface.hxx>
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <BRep_PolygonOnSurface.hxx>
#include <BRep_PolygonOnTriangulation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_TVertex.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TNaming_CopyShape.hxx>
#include <TNaming_TranslateTool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TNaming_TranslateTool,Standard_Transient)

//=======================================================================
//function : TNaming_TranslateTool
//purpose  : 
//=======================================================================
//TNaming_TranslateTool::TNaming_TranslateTool
//(const MgtBRep_TriangleMode aTriMode) : 
//myTriangleMode(aTriMode)
//{
//}
//=======================================================================
//function : Add
//purpose  : Adds S2 in S1
//=======================================================================
void TNaming_TranslateTool::Add(TopoDS_Shape& S1,
				const TopoDS_Shape& S2) const 
{
  BRep_Builder B;
  B.Add(S1,S2);
}

//=======================================================================
//function : MakeVertex
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeVertex(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeVertex(TopoDS::Vertex(S));
}

//=======================================================================
//function : MakeEdge
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeEdge(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeEdge(TopoDS::Edge(S));
}

//=======================================================================
//function : MakeWire
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeWire(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeWire(TopoDS::Wire(S));
}

//=======================================================================
//function : MakeFace
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeFace(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeFace(TopoDS::Face(S));
}

//=======================================================================
//function : MakeShell
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeShell(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeShell(TopoDS::Shell(S));
}

//=======================================================================
//function : MakeSolid
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeSolid(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeSolid(TopoDS::Solid(S));
}

//=======================================================================
//function : MakeCompSolid
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeCompSolid(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeCompSolid(TopoDS::CompSolid(S));
}

//=======================================================================
//function : MakeCompound
//purpose  : 
//=======================================================================

void  TNaming_TranslateTool::MakeCompound(TopoDS_Shape& S) const 
{
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(S));
}

//=======================================================================
// Update methods
//=======================================================================
//function : UpdateVertex
//purpose  : 
//=======================================================================

void TNaming_TranslateTool::UpdateVertex
(const TopoDS_Shape& S1,
       TopoDS_Shape& S2,
 TColStd_IndexedDataMapOfTransientTransient& aMap) const 
{
  const Handle(BRep_TVertex)& TTV1 = *((Handle(BRep_TVertex)*) &(S1.TShape()));
  const Handle(BRep_TVertex)& TTV2 = *((Handle(BRep_TVertex)*) &(S2.TShape()));

  // Point
  TTV2->Pnt(TTV1->Pnt());

  // Tolerance
  TTV2->Tolerance(TTV1->Tolerance());

  // Representations
  BRep_ListIteratorOfListOfPointRepresentation itpr(TTV1->Points());
//  cout << "Vertex List Extent = "<<  TTV1->Points().Extent()<< endl;//  == 0 ???
  BRep_ListOfPointRepresentation& lpr = TTV2->ChangePoints();
  lpr.Clear();

  while (itpr.More()) {

    const Handle(BRep_PointRepresentation)& PR1 = itpr.Value();
    Handle(BRep_PointRepresentation) PR2;
    
    if (PR1->IsPointOnCurve()) { // pointOnCurve (1)
      Handle(BRep_PointOnCurve) OC =
	new BRep_PointOnCurve(PR1->Parameter(), // the same geometry
			      PR1->Curve(),     // the same geometry
			      TNaming_CopyShape::Translate(PR1->Location(), aMap));
      PR2 = OC;
    }

    else if (PR1->IsPointOnCurveOnSurface()) { // PointOnCurveOnSurface (2)

      Handle(BRep_PointOnCurveOnSurface) OCS =
	new BRep_PointOnCurveOnSurface(PR1->Parameter(),
				       PR1->PCurve(),
				       PR1->Surface(),
				       TNaming_CopyShape::Translate(PR1->Location(), aMap));
      PR2 = OCS;
    }

    else if (PR1->IsPointOnSurface()) { // PointOnSurface (3)
      
      Handle(BRep_PointOnSurface) OS =
	new BRep_PointOnSurface(PR1->Parameter(),
				PR1->Parameter2(),
				PR1->Surface(),
				TNaming_CopyShape::Translate(PR1->Location(), aMap));
      PR2 = OS;
    }
    
//    lpr.Prepend(PR2);
    lpr.Append(PR2);
    itpr.Next();
  }
  
  UpdateShape(S1,S2);

}

//=======================================================================
//function : UpdateEdge
//purpose  : Transient->Transient
//=======================================================================

void TNaming_TranslateTool::UpdateEdge
(const TopoDS_Shape& S1,
       TopoDS_Shape& S2,
 TColStd_IndexedDataMapOfTransientTransient& aMap) const 
{
  const Handle(BRep_TEdge)&  TTE1 = *((Handle(BRep_TEdge)*) &(S1.TShape()));
  const Handle(BRep_TEdge)&  TTE2 = *((Handle(BRep_TEdge)*) &(S2.TShape()));
  // tolerance TopLoc_Location
  TTE2->Tolerance(TTE1->Tolerance());
  
  // same parameter
  TTE2->SameParameter(TTE1->SameParameter());

  // same range
  TTE2->SameRange(TTE1->SameRange());

  // Degenerated
  TTE2->Degenerated(TTE1->Degenerated());
  
  // Representations
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TTE1->Curves());
  BRep_ListOfCurveRepresentation& lcr = TTE2->ChangeCurves();
  lcr.Clear();

  Handle(BRep_GCurve) GC;
  Standard_Real f, l;
  while (itcr.More()) {

    const Handle(BRep_CurveRepresentation)& CR = itcr.Value();
    Handle(BRep_CurveRepresentation) CR2;

    GC = Handle(BRep_GCurve)::DownCast(CR);
    if (!GC.IsNull()) { // (1)
      GC->Range(f, l);      
      // CurveRepresentation is Curve3D - (1a)
      if (CR->IsCurve3D()) { 
	
	CR2 = (Handle(BRep_Curve3D)::DownCast(GC))->Copy();
	
      } 
      
      // CurveRepresentation is CurveOnSurface - (1b)
      else if (CR->IsCurveOnSurface()) { 

	if (!CR->IsCurveOnClosedSurface()) { // -(1b1) -if not closed surface

	  // CurveRepresentation is a PBRep_CurveOnSurface
	  CR2 = (Handle(BRep_CurveOnSurface)::DownCast(GC))->Copy();
	} 
	
	else { 
	  // CurveRepresentation is CurveOnClosedSurface -(1b2)
	  CR2 = (Handle(BRep_CurveOnClosedSurface)::DownCast(GC))->Copy();
	}	
      } 

      (Handle(BRep_GCurve)::DownCast(CR2))->SetRange(f, l);
    }
    
    // CurveRepresentation is CurveOn2Surfaces (2:)
    else if (CR->IsRegularity()) {
      
      CR2 = (Handle(BRep_CurveOn2Surfaces)::DownCast(CR))->Copy();
      TopLoc_Location L2 = TNaming_CopyShape::Translate(CR->Location2(), aMap);
      CR2->Location(L2);
    }
    
    // CurveRepresentation is Polygon or Triangulation  (3:)
//    else if (myTriangleMode == MgtBRep_WithTriangle) {
      
      // CurveRepresentation is Polygon3D (3a)
    else if (CR->IsPolygon3D()) { 
      
      CR2 = (Handle(BRep_Polygon3D)::DownCast(CR))->Copy();
    }

      // CurveRepresentation is PolygonOnSurface - (3b)
    else if (CR->IsPolygonOnSurface()) {
      
      // CurveRepresentation is PolygonOnClosedSurface - (3b1)
      if (CR->IsPolygonOnClosedSurface()) {
	
	CR2 = (Handle(BRep_PolygonOnClosedSurface)::DownCast(CR))->Copy();
      }
      
      // CurveRepresentation is PolygonOnSurface -  (3b2)
      else {
	
	  CR2 = (Handle(BRep_PolygonOnSurface)::DownCast(CR))->Copy();
	  
	}
    } 

      // CurveRepresentation is PolygonOnTriangulation - (3c)
    else if (CR->IsPolygonOnTriangulation()) {

      // CurveRepresentation is PolygonOnClosedTriangulation
      if (CR->IsPolygonOnClosedTriangulation()) { // (3c1)
	
	CR2 = (Handle(BRep_PolygonOnClosedTriangulation)::DownCast(CR))->Copy();
      }
      
      // CurveRepresentation is PolygonOnTriangulation - (3c2)
      else {
	
	CR2 = (Handle(BRep_PolygonOnTriangulation)::DownCast(CR))->Copy();
      } 
    }
//    }
    else {
      // jumps the curve representation
      itcr.Next();
      continue;
    }
    
    TopLoc_Location L = TNaming_CopyShape::Translate(CR->Location(), aMap);
    CR2->Location(L);
    
    Standard_NullObject_Raise_if (CR2.IsNull(), "Null CurveRepresentation");
    
//    lcr.Prepend(CR2); // add
    lcr.Append(CR2);
    itcr.Next(); 
  }
  
  UpdateShape(S1,S2);
}


//=======================================================================
//function : UpdateFace
//purpose  : Transient->Transient
//=======================================================================

void TNaming_TranslateTool::UpdateFace
(const TopoDS_Shape& S1, 
       TopoDS_Shape& S2,
 TColStd_IndexedDataMapOfTransientTransient& aMap) const 
{
  const Handle(BRep_TFace)&  TTF1 = *((Handle(BRep_TFace)*) &(S1.TShape()));
  const Handle(BRep_TFace)&  TTF2 = *((Handle(BRep_TFace)*) &(S2.TShape()));

  // natural restriction
  TTF2->NaturalRestriction(TTF1->NaturalRestriction());

  // tolerance
  TTF2->Tolerance(TTF1->Tolerance());

  // location
  TTF2->Location(TNaming_CopyShape::Translate(TTF1->Location(), aMap));

  // surface
  TTF2->Surface(TTF1->Surface());

  // Triangulation
 // if (myTriangleMode == MgtBRep_WithTriangle) { 
  TTF2->Triangulation(TTF1->Triangulation());
  //  }
  
  UpdateShape(S1,S2);

}

//=======================================================================
//function : UpdateShape
//purpose  : 
//=======================================================================

void TNaming_TranslateTool::UpdateShape
  (const TopoDS_Shape& S1, TopoDS_Shape& S2) const
{
 // Transfer the flags                                                                          
  S2.TShape()->Free(S1.TShape()->Free());                                                      
  S2.TShape()->Modified(S1.TShape()->Modified());                                                 
  S2.TShape()->Checked(S1.TShape()->Checked());                                                   
  S2.TShape()->Orientable(S1.TShape()->Orientable());                                             
  S2.TShape()->Closed(S1.TShape()->Closed());                                                     
  S2.TShape()->Infinite(S1.TShape()->Infinite());                                                 
  S2.TShape()->Convex(S1.TShape()->Convex()); 
}
