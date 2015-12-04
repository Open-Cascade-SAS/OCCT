// Created on: 1995-12-07
// Created by: Jacques GOUSSARD
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


#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListIteratorOfListOfPointRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_ListOfPointRepresentation.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_ListOfStatus.hxx>
#include <BRepCheck_Vertex.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Type.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepCheck_Vertex,BRepCheck_Result)

//=======================================================================
//function : BRepCheck_Vertex
//purpose  : 
//=======================================================================
BRepCheck_Vertex::BRepCheck_Vertex(const TopoDS_Vertex& V)
{
  Init(V);
}



//=======================================================================
//function : Minimum
//purpose  : 
//=======================================================================

void BRepCheck_Vertex::Minimum()
{
  if (!myMin) {
    // checks the existence of a point 3D
    BRepCheck_ListOfStatus thelist;
    myMap.Bind(myShape, thelist);
    myMap(myShape).Append(BRepCheck_NoError);
    myMin = Standard_True;
  }
}


//=======================================================================
//function : InContext
//purpose  : 
//=======================================================================

void BRepCheck_Vertex::InContext(const TopoDS_Shape& S)
{
  if (myMap.IsBound(S)) {
    return;
  }
  BRepCheck_ListOfStatus thelist;
  myMap.Bind(S, thelist);

//  for (TopExp_Explorer exp(S,TopAbs_VERTEX); exp.More(); exp.Next()) {
  TopExp_Explorer exp(S,TopAbs_VERTEX) ;
  for ( ; exp.More(); exp.Next()) {
    if (exp.Current().IsSame(myShape)) {
      break;
    }
  }
  if (!exp.More()) {
    BRepCheck::Add(myMap(S),BRepCheck_SubshapeNotInShape);
    return; // leaves
  }


  Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &myShape.TShape());
  const gp_Pnt& prep = TV->Pnt();
  gp_Pnt Controlp;

  TopAbs_ShapeEnum styp = S.ShapeType();
  switch (styp) {

  case TopAbs_EDGE:
    {
      // Try to find the vertex on the edge
      
      const TopoDS_Edge& E = TopoDS::Edge(S);
      TopoDS_Iterator itv(E.Oriented(TopAbs_FORWARD));
      TopoDS_Vertex VFind;
      Standard_Boolean multiple = Standard_False;
      while (itv.More()) {
	const TopoDS_Vertex& VF = TopoDS::Vertex(itv.Value());
	if (itv.Value().IsSame(myShape)) {
	  if (VFind.IsNull()) {
	    VFind = VF;
	  }
	  else {
	    if ((VFind.Orientation() == TopAbs_FORWARD && 
		 VF.Orientation() == TopAbs_REVERSED) ||
		(VFind.Orientation() == TopAbs_REVERSED &&
		 VF.Orientation() == TopAbs_FORWARD)) {
	      // the vertex on the edge is at once F and R
	      multiple = Standard_True; 
	    }
	    if (VFind.Orientation() != TopAbs_FORWARD && 
		VFind.Orientation() != TopAbs_REVERSED) {
	      if (VF.Orientation() == TopAbs_FORWARD ||
		  VF.Orientation() == TopAbs_REVERSED) {
		VFind = VF;
	      }
	    }
	  }
	}
	itv.Next();
      }

      // VFind is not null for sure
      TopAbs_Orientation orv = VFind.Orientation();

      Standard_Real Tol  = BRep_Tool::Tolerance(TopoDS::Vertex(myShape));
      Tol = Max(Tol,BRep_Tool::Tolerance(E)); // to check
      Tol *= Tol;

      Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
      BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
      const TopLoc_Location& Eloc = E.Location();

      BRep_ListIteratorOfListOfPointRepresentation itpr;
      while (itcr.More()) {
	// For each CurveRepresentation, the provided parameter is checked
	const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
	const TopLoc_Location& loc = cr->Location();
	TopLoc_Location L = (Eloc * loc).Predivided(myShape.Location());

	if (cr->IsCurve3D()) {
	  const Handle(Geom_Curve)& C = cr->Curve3D();
	  if (!C.IsNull()) { // edge non degenerated
	    itpr.Initialize(TV->Points());
	    while (itpr.More()) {
	      const Handle(BRep_PointRepresentation)& pr = itpr.Value();
	      if (pr->IsPointOnCurve(C,L)) {
		Controlp = C->Value(pr->Parameter());
		Controlp.Transform(L.Transformation());
		if (prep.SquareDistance(Controlp)> Tol) {
		  BRepCheck::Add(myMap(S),BRepCheck_InvalidPointOnCurve);
		}
	      }
	      itpr.Next();
	    }
	    if (orv == TopAbs_FORWARD || orv == TopAbs_REVERSED) {
	      Handle(BRep_GCurve) GC (Handle(BRep_GCurve)::DownCast (cr));
	      if (orv == TopAbs_FORWARD || multiple) {
		Controlp = C->Value(GC->First());
		Controlp.Transform(L.Transformation());
		if (prep.SquareDistance(Controlp)> Tol) {
		  BRepCheck::Add(myMap(S),BRepCheck_InvalidPointOnCurve);
		}
	      }
	      if (orv == TopAbs_REVERSED || multiple) {
		Controlp = C->Value(GC->Last());
		Controlp.Transform(L.Transformation());
		if (prep.SquareDistance(Controlp)> Tol) {
		  BRepCheck::Add(myMap(S),BRepCheck_InvalidPointOnCurve);
		}
	      }
	    }
	  }
	}
	else if (cr->IsCurveOnSurface()) {
	  const Handle(Geom_Surface)& Su = cr->Surface();
	  const Handle(Geom2d_Curve)& PC = cr->PCurve();
	  Handle(Geom2d_Curve) PC2;
	  if (cr->IsCurveOnClosedSurface()) {
	    PC2 = cr->PCurve2();
	  }
	  itpr.Initialize(TV->Points());
	  while (itpr.More()) {
	    const Handle(BRep_PointRepresentation)& pr = itpr.Value();
	    if (pr->IsPointOnCurveOnSurface(PC,Su,L)) {
	      gp_Pnt2d p2d = PC->Value(pr->Parameter());
	      Controlp = Su->Value(p2d.X(),p2d.Y());
	      Controlp.Transform(L.Transformation());
	      if (prep.SquareDistance(Controlp)> Tol) {
		BRepCheck::Add(myMap(S),
			       BRepCheck_InvalidPointOnCurveOnSurface);
	      }
	    }
	    if (!PC2.IsNull() && pr->IsPointOnCurveOnSurface(PC2,Su,L)) {
	      gp_Pnt2d p2d = PC2->Value(pr->Parameter());
	      Controlp = Su->Value(p2d.X(),p2d.Y());
	      Controlp.Transform(L.Transformation());
	      if (prep.SquareDistance(Controlp)> Tol) {
		BRepCheck::Add(myMap(S),
			       BRepCheck_InvalidPointOnCurveOnSurface);
	      }
	    }
	    itpr.Next();
	  }
	}
	itcr.Next();
      }
      if (myMap(S).IsEmpty()) {
	myMap(S).Append(BRepCheck_NoError);
      }

    }
    break;

  case TopAbs_FACE:
    {

      Handle(BRep_TFace)& TF = *((Handle(BRep_TFace)*) &S.TShape());
      const TopLoc_Location& Floc = S.Location();
      const TopLoc_Location& TFloc = TF->Location();
      const Handle(Geom_Surface)& Su = TF->Surface();
      TopLoc_Location L = (Floc * TFloc).Predivided(myShape.Location());

      Standard_Real Tol  = BRep_Tool::Tolerance(TopoDS::Vertex(myShape));
      Tol = Max(Tol,BRep_Tool::Tolerance(TopoDS::Face(S))); // to check
      Tol *= Tol;

      BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());
      while (itpr.More()) {
	const Handle(BRep_PointRepresentation)& pr = itpr.Value();
	if (pr->IsPointOnSurface(Su,L)) {
	  Controlp = Su->Value(pr->Parameter(),pr->Parameter2());
	  Controlp.Transform(L.Transformation());
	  if (prep.SquareDistance(Controlp)> Tol) {
	    BRepCheck::Add(myMap(S),BRepCheck_InvalidPointOnSurface);
	  }
	}
	itpr.Next();
      }
      if (myMap(S).IsEmpty()) {
	myMap(S).Append(BRepCheck_NoError);
      }
    }

  default:
    break;

  }

}


//=======================================================================
//function : Blind
//purpose  : 
//=======================================================================

void BRepCheck_Vertex::Blind()
{
  if (myBlind) {
    return;
  }
//   modified by NIZHNY-MKK  Fri May  7 16:43:38 2004.BEGIN
//   The body of this function is removed because of its useless 
//   (see specification "Substitution existing set of evaluation DRAW commands to one").

//   Check all the representations  of the vertex. (i-e checks the TVertex
//   BRepCheck_ListOfStatus& lst = myMap(myShape);
//   lst.Clear(); // there was NoError...

//   Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &myShape.TShape());
//   const gp_Pnt& prep = TV->Pnt();
//   Standard_Real Tol  = BRep_Tool::Tolerance(TopoDS::Vertex(myShape));
//   Tol *= Tol;

//   gp_Pnt Controlp;
//   BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());
//   BRepCheck_Status stat=BRepCheck_NoError;
//   while (itpr.More()) {
//     const Handle(BRep_PointRepresentation)& pr = itpr.Value();
//     const TopLoc_Location& loc = pr->Location();
//     if (pr->IsPointOnCurve()) {
//       Controlp = pr->Curve()->Value(pr->Parameter());
//       stat = BRepCheck_InvalidPointOnCurve;
//     }
//     else if (pr->IsPointOnCurveOnSurface()) {
//       gp_Pnt2d Puv = pr->PCurve()->Value(pr->Parameter());
//       Controlp = pr->Surface()->Value(Puv.X(),Puv.Y());
//       stat = BRepCheck_InvalidPointOnCurveOnSurface;
//     }
//     else if (pr->IsPointOnSurface()) {
//       Controlp = pr->Surface()->Value(pr->Parameter(),pr->Parameter2());
//       stat = BRepCheck_InvalidPointOnSurface;
//     }
//     Controlp.Transform(loc.Transformation());
//     if (prep.SquareDistance(Controlp) > Tol) {
//       BRepCheck::Add(lst,stat);
//     }
//     itpr.Next();
//   }

//   if (lst.IsEmpty()) {
//     lst.Append(BRepCheck_NoError);
//   }
// modified by NIZHNY-MKK  Fri May  7 16:43:45 2004.END
  myBlind = Standard_True;
}


//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real BRepCheck_Vertex::Tolerance()
{

  // Check all the representations  of the vertex. (i-e checks the TVertex
  Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*) &myShape.TShape());
  const gp_Pnt& prep = TV->Pnt();
  Standard_Real Tol  = BRep_Tool::Tolerance(TopoDS::Vertex(myShape));
  Tol *= Tol;

  gp_Pnt Controlp;
  Controlp = prep;
  BRep_ListIteratorOfListOfPointRepresentation itpr(TV->Points());
  while (itpr.More()) {
    const Handle(BRep_PointRepresentation)& pr = itpr.Value();
    const TopLoc_Location& loc = pr->Location();
    if (pr->IsPointOnCurve()) {
      if (!pr->Curve().IsNull())
	Controlp = pr->Curve()->Value(pr->Parameter());
    }
    else if (pr->IsPointOnCurveOnSurface()) {
      gp_Pnt2d Puv = pr->PCurve()->Value(pr->Parameter());
      Controlp = pr->Surface()->Value(Puv.X(),Puv.Y());
    }
    else if (pr->IsPointOnSurface()) {
      Controlp = pr->Surface()->Value(pr->Parameter(),pr->Parameter2());
    }
    Controlp.Transform(loc.Transformation());
    if (prep.SquareDistance(Controlp) > Tol) {
      Tol = prep.SquareDistance(Controlp);
    }
    itpr.Next();
  }
  return sqrt(Tol*1.05);
}


