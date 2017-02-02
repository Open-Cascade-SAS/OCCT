// Created on: 1994-08-25
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <BRepTools_TrsfModification.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <gp.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_TrsfForm.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_TrsfModification,BRepTools_Modification)

//=======================================================================
//function : BRepTools_TrsfModification
//purpose  : 
//=======================================================================
BRepTools_TrsfModification::BRepTools_TrsfModification(const gp_Trsf& T) :
myTrsf(T)
{
}


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

gp_Trsf& BRepTools_TrsfModification::Trsf ()
{
  return myTrsf;
}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewSurface
      (const TopoDS_Face& F,
       Handle(Geom_Surface)& S,
       TopLoc_Location& L,
       Standard_Real& Tol,
       Standard_Boolean& RevWires,
       Standard_Boolean& RevFace)
{
  S = BRep_Tool::Surface(F,L);
  Tol = BRep_Tool::Tolerance(F);
  Tol *= Abs(myTrsf.ScaleFactor());
  RevWires = Standard_False;
  RevFace = myTrsf.IsNegative();

  gp_Trsf LT = L.Transformation();
  LT.Invert();
  LT.Multiply(myTrsf);
  LT.Multiply(L.Transformation());

  S = Handle(Geom_Surface)::DownCast(S->Transformed(LT));
  
  return Standard_True;
}


//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewCurve
    (const TopoDS_Edge& E, 
     Handle(Geom_Curve)& C,
     TopLoc_Location& L, 
     Standard_Real& Tol)
{
  Standard_Real f,l;
  C = BRep_Tool::Curve(E,L,f,l);

  Tol = BRep_Tool::Tolerance(E);
  Tol *= Abs(myTrsf.ScaleFactor());

  gp_Trsf LT = L.Transformation();
  LT.Invert();
  LT.Multiply(myTrsf);
  LT.Multiply(L.Transformation());

  if (!C.IsNull()) {
    C = Handle(Geom_Curve)::DownCast(C->Transformed(LT));
  }

  return Standard_True;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewPoint
    (const TopoDS_Vertex& V, 
     gp_Pnt& P, 
     Standard_Real& Tol)
{
  P = BRep_Tool::Pnt(V);
  Tol = BRep_Tool::Tolerance(V);
  Tol *= Abs(myTrsf.ScaleFactor());
  P.Transform(myTrsf);

  return Standard_True;
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewCurve2d
    (const TopoDS_Edge& E, 
     const TopoDS_Face& F, 
     const TopoDS_Edge&, 
     const TopoDS_Face&, 
     Handle(Geom2d_Curve)& C,
     Standard_Real& Tol)
{
  TopLoc_Location loc;
  Tol = BRep_Tool::Tolerance(E);
  Standard_Real scale = myTrsf.ScaleFactor();
  Tol *= Abs(scale);
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,loc);
  GeomAdaptor_Surface GAsurf(S);
  if (GAsurf.GetType() == GeomAbs_Plane)
    return Standard_False;

  Standard_Real f,l;
  Handle(Geom2d_Curve) NewC = BRep_Tool::CurveOnSurface(E,F,f,l);
  if (NewC.IsNull())
    return Standard_False;
  
  Standard_Real newf,newl;

  Handle(Standard_Type) TheType = NewC->DynamicType();
    
  if ( TheType == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(NewC);
    NewC = TC->BasisCurve();
  }

  Standard_Real fc = NewC->FirstParameter(), lc = NewC->LastParameter();

  if(!NewC->IsPeriodic()) {
    if(fc - f > Precision::PConfusion()) f = fc;
    if(l - lc > Precision::PConfusion()) l = lc;
  }

  newf = f;
  newl = l;
  if (Abs(scale) != 1.) {
    
    NewC = new Geom2d_TrimmedCurve(NewC,f,l);
    gp_GTrsf2d gtrsf = S->ParametricTransformation(myTrsf);
    
    if ( gtrsf.Form() != gp_Identity) {
      NewC = GeomLib::GTransform(NewC,gtrsf);
      if (NewC.IsNull()) {
	throw Standard_DomainError("TrsfModification:Error in NewCurve2d");
	}
      newf = NewC->FirstParameter();
      newl = NewC->LastParameter();
    }
  }
  // il faut parfois recadrer les ranges 3d / 2d
  TopoDS_Vertex V1, V2;
  TopExp::Vertices(E,V1,V2);
  TopoDS_Shape initEFOR = E.Oriented(TopAbs_FORWARD); // skl
  TopoDS_Edge EFOR = TopoDS::Edge(initEFOR/*E.Oriented(TopAbs_FORWARD)*/); //skl
  Standard_Real aTolV;
  NewParameter(V1, EFOR, f, aTolV);
  NewParameter(V2, EFOR, l, aTolV);
  GeomLib::SameRange(Precision::PConfusion(), NewC, newf, newl, f, l, C);
  
  return Standard_True;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_TrsfModification::NewParameter
   (const TopoDS_Vertex& V, 
    const TopoDS_Edge& E, 
    Standard_Real& P, 
    Standard_Real& Tol)
{
  if (V.IsNull()) return Standard_False; // infinite edge may have Null vertex
  
  TopLoc_Location loc;
  Tol = BRep_Tool::Tolerance(V);
  Tol *= Abs(myTrsf.ScaleFactor());
  P = BRep_Tool::Parameter(V,E);

  Standard_Real f,l;

  Handle(Geom_Curve) C = BRep_Tool::Curve(E,loc,f,l);
  if (!C.IsNull()) {
    P = C->TransformedParameter(P,myTrsf);
  }

  return Standard_True;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepTools_TrsfModification::Continuity
  (const TopoDS_Edge& E,
   const TopoDS_Face& F1,
   const TopoDS_Face& F2,
   const TopoDS_Edge&,
   const TopoDS_Face&,
   const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E,F1,F2);
}


