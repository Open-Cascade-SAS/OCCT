// Created on: 1993-12-06
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRepBlend_BlendTool.ixx>

#include <BRepClass_FaceClassifier.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <Geom2dInt_GInter.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <BRepBlend_HCurve2dTool.hxx>


//=======================================================================
//function : Project 
//purpose  : Projection orthogonal d'un point sur une courbe
// pmn 8/10/98 : On retourne toujours une distance. (BUC60360)
//=======================================================================
Standard_Boolean BRepBlend_BlendTool::Project(const gp_Pnt2d& P,
                                              const Handle(Adaptor3d_HSurface)&,
                                              const Handle(Adaptor2d_HCurve2d)& C,
                                              Standard_Real& Paramproj,
                                              Standard_Real& Dist)
{
  Paramproj =  BRepBlend_HCurve2dTool::FirstParameter(C);
  gp_Pnt2d P2d;
  BRepBlend_HCurve2dTool::D0(C, Paramproj, P2d);
  Dist = P2d.Distance(P);

  const Standard_Real t =  BRepBlend_HCurve2dTool::LastParameter(C);
  BRepBlend_HCurve2dTool::D0(C, t, P2d);
  if (P2d.Distance(P) < Dist) {
    Paramproj = t;
    Dist = P2d.Distance(P);
  }

  const Standard_Real epsX = 1.e-8;
  const Standard_Integer Nbu = 20;
  const Standard_Real Tol = 1.e-5;
  Extrema_EPCOfExtPC2d extrema(P, C->Curve2d(), Nbu, epsX, Tol);
  if (!extrema.IsDone())
    return Standard_True;

  const Standard_Integer Nbext = extrema.NbExt(); 
  Standard_Real aDist2 = Dist * Dist;
  for (Standard_Integer i=1; i<=Nbext; i++) {
    if (extrema.SquareDistance(i) < aDist2) {
      aDist2 = extrema.SquareDistance(i);
      Paramproj = extrema.Point(i).Parameter();
    }
  }
  Dist = sqrt (aDist2);

  return Standard_True;
}

//=======================================================================
//function : Inters 
//purpose  : Intersection d'un segment avec une courbe
//=======================================================================
Standard_Boolean BRepBlend_BlendTool::Inters(const gp_Pnt2d& P1,
                                             const gp_Pnt2d& P2,
                                             const Handle(Adaptor3d_HSurface)&,
                                             const Handle(Adaptor2d_HCurve2d)& C,
                                             Standard_Real& Param,
                                             Standard_Real& Dist)
{
  const Standard_Real Tol = 1.e-8;
  const gp_Vec2d v(P1,P2);
  const Standard_Real mag = v.Magnitude();
  if(mag < Tol) return Standard_False;

  gp_Dir2d d(v);
  Handle(Geom2d_Line) bid = new Geom2d_Line(P1,d);
  Geom2dAdaptor_Curve seg(bid,-0.01*mag,1.01*mag);

  Geom2dInt_GInter inter(seg,C->Curve2d(),Tol,Tol);
  if (!inter.IsDone())
    return Standard_False;

  const Standard_Integer Nbint = inter.NbPoints();
  if (Nbint == 0)
    return Standard_False;

  IntRes2d_IntersectionPoint ip = inter.Point(1);
  Param = ip.ParamOnSecond();
  Dist = P1.Distance(ip.Value());
  return Standard_True;
}

Standard_Integer BRepBlend_BlendTool::NbSamplesV
  (const Handle(Adaptor3d_HSurface)& S,
   const Standard_Real u1,
   const Standard_Real u2)
{
  return 10;
}

Standard_Integer BRepBlend_BlendTool::NbSamplesU
  (const Handle(Adaptor3d_HSurface)& S,
   const Standard_Real u1,
   const Standard_Real u2)
{
  return 10;
}

void BRepBlend_BlendTool::Bounds(const Handle(Adaptor2d_HCurve2d)& A,
                                 Standard_Real& Ufirst,
                                 Standard_Real& Ulast)
{
  Ufirst = BRepBlend_HCurve2dTool::FirstParameter(A);
  Ulast  = BRepBlend_HCurve2dTool::LastParameter(A);
}
