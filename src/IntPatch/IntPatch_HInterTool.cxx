// Created on: 1995-07-02
// Created by: Laurent BUCHARD
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


#include <IntPatch_HInterTool.ixx>

#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>

#include <GeomAbs_SurfaceType.hxx>
#include <Standard_OutOfRange.hxx>

#include <Handle_Geom_BezierSurface.hxx>
#include <Handle_Geom_BSplineSurface.hxx>
#include <Handle_Geom2d_BezierCurve.hxx>
#include <Handle_Geom2d_BSplineCurve.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>

static Standard_Real uinf,vinf,usup,vsup;


Standard_Integer IntPatch_HInterTool::NbSamplesV (const Handle(Adaptor3d_HSurface)& S,
                                                  const Standard_Real, const Standard_Real)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbVPoles());
    case GeomAbs_BSplineSurface: 
    {
      Standard_Integer nbs = S->NbVKnots();
      nbs *= S->VDegree();
      if (!S->IsVRational()) nbs *= 2;
      if (nbs < 4) nbs = 4;
	  return nbs;
    }
    break;
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfRevolution:
    case GeomAbs_SurfaceOfExtrusion: return 15;
  }
  return 10;
}

Standard_Integer IntPatch_HInterTool::NbSamplesU (const Handle(Adaptor3d_HSurface)& S,
                                                  const Standard_Real, const Standard_Real)
{
  switch (S->GetType())
  {
    case GeomAbs_Plane: return 2;
    case GeomAbs_BezierSurface: return (3 + S->NbUPoles());
    case GeomAbs_BSplineSurface:
    {
      Standard_Integer nbs = S->NbUKnots();
      nbs *= S->UDegree();
      if (!S->IsURational()) nbs *= 2;
      if (nbs < 4) nbs = 4;
      return nbs;
	}
    case GeomAbs_Torus: return 20;
    //case GeomAbs_Cylinder:
    //case GeomAbs_Cone:
    //case GeomAbs_Sphere:
    //case GeomAbs_SurfaceOfRevolution:
    //case GeomAbs_SurfaceOfExtrusion: return 10;
  }
  return 10;
}

Standard_Integer IntPatch_HInterTool::NbSamplePoints (const Handle(Adaptor3d_HSurface)& S)
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
  const Standard_Integer m = (1+NbSamplesU(S,uinf,usup)/2) * (1+NbSamplesV(S,vinf,vsup)/2);
  return(m);
}

void IntPatch_HInterTool::SamplePoint (const Handle(Adaptor3d_HSurface)& S,
                                       const Standard_Integer Index,
                                       Standard_Real& U,
                                       Standard_Real& V )
{
  Standard_Integer nbIntU = 1+NbSamplesU(S,uinf,usup);
  nbIntU>>=1;
  Standard_Integer nbIntV = 1+NbSamplesV(S,vinf,vsup);     
  nbIntV>>=1;
  
  if(nbIntU * nbIntV >5) {
    
    Standard_Integer NV = (Index-1) / nbIntU;
    Standard_Integer NU = (Index-1) - NV * nbIntU;
    Standard_Real du = ((usup-uinf)/((Standard_Real)(nbIntU+1)));
    Standard_Real dv = ((vsup-vinf)/((Standard_Real)(nbIntV+1)));
    
    Standard_Integer perturb=(NU+NV) & 3;

    //-- petite perturbation pou eviter les pbs de symetrie avec Les recherches de points int. 

    switch(perturb) { 
    case 1: dv*=1.001;  dv*=0.999; break;
    case 2: du*=1.001;  dv*=1.001; break;
    case 3: du*=0.999;             break;
    default: break;
    }
    
    U = uinf + du*(Standard_Real)(NU+1);
    V = vinf + dv*(Standard_Real)(NV+1);
    return;
  }
  
  
  switch (Index) {
  case 1:
    U = 0.76*uinf + 0.24*usup;
    V = 0.74*vinf + 0.26*vsup;
    break;
  case 2:
    U = 0.73*uinf + 0.27*usup;
    V = 0.24*vinf + 0.76*vsup;
    break;
  case 3:
    U = 0.25*uinf + 0.75*usup;
    V = 0.76*vinf + 0.24*vsup;
    break;
  case 4:
    U = 0.26*uinf + 0.74*usup;
    V = 0.25*vinf + 0.75*vsup;
    break;
  default:
    U = 0.51*uinf+0.49*usup; 
    V = 0.49*vinf+0.51*vsup;
  }
}


Standard_Integer IntPatch_HInterTool::NbSamplesOnArc (const Handle(Adaptor2d_HCurve2d)& A)
{
  GeomAbs_CurveType CurveType  = A->GetType();

//  Standard_Real nbsOnC = 5;
  Standard_Integer nbsOnC = 5;
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
    //-- Handle_Geom2d_BSplineCurve& BSC=A->BSpline();
    nbsOnC = 2 + A->NbKnots() * A->Degree();
    break;
  }
  default:
    nbsOnC = 10;
  } 
  return(nbsOnC);
}

void IntPatch_HInterTool::Bounds(const Handle(Adaptor2d_HCurve2d)& A,
                                 Standard_Real& Ufirst,
                                 Standard_Real& Ulast)
{
  Ufirst = A->FirstParameter();
  Ulast  = A->LastParameter();
}

Standard_Boolean IntPatch_HInterTool::Project (const Handle(Adaptor2d_HCurve2d)& C,
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

Standard_Real IntPatch_HInterTool::Tolerance (const Handle(Adaptor3d_HVertex)& V,
                                              const Handle(Adaptor2d_HCurve2d)& C)
{
  return V->Resolution(C);
}

Standard_Real IntPatch_HInterTool::Parameter (const Handle(Adaptor3d_HVertex)& V,
                                              const Handle(Adaptor2d_HCurve2d)& C)
{
  return V->Parameter(C);
}

Standard_Boolean IntPatch_HInterTool::HasBeenSeen(const Handle(Adaptor2d_HCurve2d)&)
{
  return Standard_False;
}

Standard_Integer IntPatch_HInterTool::NbPoints(const Handle(Adaptor2d_HCurve2d)&)
{
  return 0;
}

void IntPatch_HInterTool::Value(const Handle(Adaptor2d_HCurve2d)&,
                                const Standard_Integer,
                                gp_Pnt&,
                                Standard_Real&,
                                Standard_Real&)
{
  Standard_OutOfRange::Raise();
}

Standard_Boolean IntPatch_HInterTool::IsVertex(const Handle(Adaptor2d_HCurve2d)&,
                                               const Standard_Integer)
{
  return Standard_False;
}

void IntPatch_HInterTool::Vertex(const Handle(Adaptor2d_HCurve2d)&,
                                 const Standard_Integer,
                                 Handle(Adaptor3d_HVertex)&)
{
  Standard_OutOfRange::Raise();
}

Standard_Integer IntPatch_HInterTool::NbSegments(const Handle(Adaptor2d_HCurve2d)&)
{
  return 0;
}

Standard_Boolean IntPatch_HInterTool::HasFirstPoint (const Handle(Adaptor2d_HCurve2d)&,
                                                     const Standard_Integer,
                                                     Standard_Integer&)
{
  Standard_OutOfRange::Raise();
  return Standard_False;
}

Standard_Boolean IntPatch_HInterTool::HasLastPoint (const Handle(Adaptor2d_HCurve2d)&,
                                                    const Standard_Integer,
                                                    Standard_Integer&)
{
  Standard_OutOfRange::Raise();
  return Standard_False;
}

Standard_Boolean IntPatch_HInterTool::IsAllSolution (const Handle(Adaptor2d_HCurve2d)&)
{
  return Standard_False;
}
