// Created on: 1995-07-02
// Created by: Laurent BUCHARD
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Contap_HContTool.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

static Standard_Real uinf,vinf,usup,vsup;

Standard_Integer Contap_HContTool::NbSamplesV
(const Handle(Adaptor3d_HSurface)& S,
 const Standard_Real ,
 const Standard_Real )
{
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S->GetType();
  switch(typS) { 
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface: 
    {
      nbs =  3 + S->NbVPoles();
    }
    break;
  case GeomAbs_BSplineSurface: 
    {
      //-- Handle(Geom_BSplineSurface)& HBS=S->BSpline();
      nbs = S->NbVKnots();
      nbs*= S->VDegree();
      if(nbs < 2) nbs=2;

    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 15;
    }
    break;

  default: 
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

Standard_Integer Contap_HContTool::NbSamplesU
(const Handle(Adaptor3d_HSurface)& S,
 const Standard_Real ,
 const Standard_Real )
{
  Standard_Integer nbs;
  GeomAbs_SurfaceType typS = S->GetType();
  switch(typS) { 
  case GeomAbs_Plane:
    {
      nbs = 2;
    }
    break;
  case GeomAbs_BezierSurface: 
    {
      nbs =  3 + S->NbUPoles();
    }
    break;
  case GeomAbs_BSplineSurface: 
    {
      //-- Handle(Geom_BSplineSurface)& HBS=S->BSpline();
      nbs = S->NbUKnots();
      nbs*= S->UDegree();
      if(nbs < 2) nbs=2;

    }
    break;
  case GeomAbs_Torus: 
    {
      nbs = 20;
    }
    break;
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion:
    {
      nbs = 10;
    }
    break;

  default: 
    {
      nbs = 10;
    }
    break;
  }
  return(nbs);
}

Standard_Integer Contap_HContTool::NbSamplePoints
(const Handle(Adaptor3d_HSurface)& S)
{
  uinf = S->FirstUParameter();
  usup = S->LastUParameter();
  vinf = S->FirstVParameter();
  vsup = S->LastVParameter();

  if (usup < uinf) {
    Standard_Real temp = uinf;
    uinf = usup;
    usup = temp;
  }
  if (vsup < vinf) {
    Standard_Real temp = vinf;
    vinf = vsup;
    vsup = temp;
  }
  if (uinf == RealFirst() && usup == RealLast()) {
    uinf = -1.e5;
    usup =  1.e5;
  }
  else if (uinf == RealFirst()) {
    uinf = usup - 2.e5;
  }
  else if (usup == RealLast()) {
    usup = uinf + 2.e5;
  }

  if (vinf == RealFirst() && vsup == RealLast()) {
    vinf = -1.e5;
    vsup =  1.e5;
  }
  else if (vinf == RealFirst()) {
    vinf = vsup - 2.e5;
  }
  else if (vsup == RealLast()) {
    vsup = vinf + 2.e5;
  }
  if(S->GetType() ==   GeomAbs_BSplineSurface) { 
    Standard_Integer m = (NbSamplesU(S,uinf,usup)/3) * (NbSamplesV(S,vinf,vsup)/3);
    if(m>5) return(m); 
    else return(5);
  }
  else 
    return 5;
}

void Contap_HContTool::SamplePoint (const Handle(Adaptor3d_HSurface)& S,
                                    const Standard_Integer Index,
                                    Standard_Real& U,
                                    Standard_Real& V )
{
  if(S->GetType() ==   GeomAbs_BSplineSurface) {
    Standard_Integer nbIntU = NbSamplesU(S,uinf,usup)/3;
    Standard_Integer nbIntV = NbSamplesV(S,vinf,vsup)/3;
    if(nbIntU * nbIntV >5) { 
      Standard_Integer indU = (Index-1)/nbIntU;                  //----   0 --> nbIntV
      Standard_Integer indV = (Index-1) - indU*nbIntU;           //----   0 --> nbIntU

      U = uinf + ((usup-uinf)/((Standard_Real)(nbIntU+1)))*(Standard_Real)(indU+1);
      V = vinf + ((vsup-vinf)/((Standard_Real)(nbIntV+2)))*(Standard_Real)(indV+1);

      //-- cout<<"Index :"<<Index<<"  uinf:"<<uinf<<"  usup:"<<usup<<"  vinf:"<<vinf<<" vsup:"<<vsup<<"  ";
      //-- cout<<"  ("<<indU<<"/"<<nbIntU<<" ->U:"<<U<<"  ";
      //-- cout<<"  ("<<indV<<"/"<<nbIntV<<" ->V:"<<V<<endl;
      return;
    }
  }

  switch (Index) {
  case 1:
    U = 0.75*uinf + 0.25*usup; //0.25;
    V = 0.75*vinf + 0.25*vsup; //0.25;
    break;
  case 2:
    U = 0.75*uinf + 0.25*usup; //0.25;
    V = 0.25*vinf + 0.75*vsup; //0.75;
    break;
  case 3:
    U = 0.25*uinf + 0.75*usup; //0.75;
    V = 0.75*vinf + 0.25*vsup; //0.25;
    break;
  case 4:
    U = 0.25*uinf + 0.75*usup; //0.75;
    V = 0.25*vinf + 0.75*vsup; //0.75;
    break;
  default:
    U = 0.5*(uinf+usup); //0.5;
    V = 0.5*(vinf+vsup); //0.5;
  }
}


Standard_Integer Contap_HContTool::NbSamplesOnArc
(const Handle(Adaptor2d_HCurve2d)& A) { 

  GeomAbs_CurveType CurveType  = A->GetType();

  Standard_Real nbsOnC = 5;
  switch(CurveType) { 
  case GeomAbs_Line:
    nbsOnC = 2;
    break;
  case GeomAbs_Circle:
  case GeomAbs_Ellipse:
  case GeomAbs_Hyperbola:
  case GeomAbs_Parabola:
    nbsOnC = 10;
    break;
  case GeomAbs_BezierCurve:
    nbsOnC = A->NbPoles();
    break;
  case GeomAbs_BSplineCurve: { 
    //-- Handle(Geom2d_BSplineCurve)& BSC=A->BSpline();
    nbsOnC = 2 + A->NbKnots() * A->Degree();
    break;
                             }
  default:
    nbsOnC = 10;
  } 
  return (Standard_Integer)(nbsOnC);
}


void Contap_HContTool::Bounds(const Handle(Adaptor2d_HCurve2d)& A,
                              Standard_Real& Ufirst,
                              Standard_Real& Ulast)
{
  Ufirst = A->FirstParameter();
  Ulast  = A->LastParameter();
}


Standard_Boolean Contap_HContTool::Project (const Handle(Adaptor2d_HCurve2d)& C,
                                            const gp_Pnt2d& P,
                                            Standard_Real& Paramproj,
                                            gp_Pnt2d& Ptproj)

{

  Standard_Real epsX = 1.e-8;
  Standard_Integer Nbu = 20;
  Standard_Real Tol = 1.e-5;
  Standard_Real Dist2;

  Extrema_EPCOfExtPC2d extrema(P,C->Curve2d(),Nbu,epsX,Tol);
  if (!extrema.IsDone()) {
    return Standard_False;
  }
  Standard_Integer Nbext = extrema.NbExt();
  if (Nbext == 0) {
    return Standard_False;
  }
  Standard_Integer indexmin = 1;
  Dist2 = extrema.SquareDistance(1);
  for (Standard_Integer i=2; i<=Nbext; i++) {
    if (extrema.SquareDistance(i) < Dist2) {
      indexmin = i;
      Dist2 = extrema.SquareDistance(i);
    }
  }
  Paramproj = extrema.Point(indexmin).Parameter();
  Ptproj = extrema.Point(indexmin).Value();
  return Standard_True;
}


Standard_Real Contap_HContTool::Tolerance (const Handle(Adaptor3d_HVertex)& V,
                                           const Handle(Adaptor2d_HCurve2d)& C)
{
  //  return BRepAdaptor2d_Curve2dTool::Resolution(C,BRep_Tool::Tolerance(V));
  return V->Resolution(C);
}

Standard_Real Contap_HContTool::Parameter (const Handle(Adaptor3d_HVertex)& V,
                                           const Handle(Adaptor2d_HCurve2d)& C)
{
  //  return BRep_Tool::Parameter(V,C.Edge());
  return V->Parameter(C);
}



Standard_Boolean Contap_HContTool::HasBeenSeen
(const Handle(Adaptor2d_HCurve2d)&)
{
  return Standard_False;
}

Standard_Integer Contap_HContTool::NbPoints(const Handle(Adaptor2d_HCurve2d)&)
{
  return 0;
}

void Contap_HContTool::Value(const Handle(Adaptor2d_HCurve2d)&,
                             const Standard_Integer,
                             gp_Pnt&,
                             Standard_Real&,
                             Standard_Real&)
{
  throw Standard_OutOfRange();
}

Standard_Boolean Contap_HContTool::IsVertex(const Handle(Adaptor2d_HCurve2d)&,
                                            const Standard_Integer)
{
  return Standard_False;
}

void Contap_HContTool::Vertex(const Handle(Adaptor2d_HCurve2d)&,
                              const Standard_Integer,
                              Handle(Adaptor3d_HVertex)&)
{
  throw Standard_OutOfRange();
}

Standard_Integer Contap_HContTool::NbSegments(const Handle(Adaptor2d_HCurve2d)&)
{
  return 0;
}

Standard_Boolean Contap_HContTool::HasFirstPoint
(const Handle(Adaptor2d_HCurve2d)&,
 const Standard_Integer,
 Standard_Integer&)
{
  throw Standard_OutOfRange();
}

Standard_Boolean Contap_HContTool::HasLastPoint
(const Handle(Adaptor2d_HCurve2d)&,
 const Standard_Integer,
 Standard_Integer&)
{
  throw Standard_OutOfRange();
}

Standard_Boolean Contap_HContTool::IsAllSolution
(const Handle(Adaptor2d_HCurve2d)&)

{
  return Standard_False;
}

