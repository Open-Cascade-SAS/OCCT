// Created on: 1993-09-29
// Created by: Isabelle GRIGNON
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


#include <BRepMesh_GeomTool.ixx>

#include <BRepMesh_ShapeTool.hxx>
#include <TopAbs_Orientation.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <CSLib.hxx>
#include <Precision.hxx>
#include <Adaptor3d_IsoCurve.hxx>
#include <BRepAdaptor_Curve.hxx>

BRepMesh_GeomTool::BRepMesh_GeomTool(BRepAdaptor_Curve& C,
				       const Standard_Real Ufirst,
				       const Standard_Real Ulast,
				       const Standard_Real AngDefl,
				       const Standard_Real Deflection,
				       const Standard_Integer nbpmin) :
       pnts(C,Ufirst,Ulast,AngDefl,Deflection, nbpmin),
//       pnts(C,Deflection,Ufirst,Ulast),
       parametric(GeomAbs_NoneIso)
{
}
//
BRepMesh_GeomTool::BRepMesh_GeomTool(const Handle(BRepAdaptor_HSurface)& S,
				       const Standard_Real ParamIso,
				       const GeomAbs_IsoType Type,
				       const Standard_Real Ufirst,
				       const Standard_Real Ulast,
				       const Standard_Real AngDefl,
				       const Standard_Real Deflection,
				       const Standard_Integer nbpmin) :
       parametric(Type)
{
  Adaptor3d_IsoCurve Iso(S, Type,ParamIso,Ufirst,Ulast);
  pnts.Initialize(Iso,Ufirst,Ulast,AngDefl,Deflection, nbpmin);
//  pnts.Initialize(Iso,Deflection,Ufirst,Ulast);
}

Standard_Integer BRepMesh_GeomTool::AddPoint(const gp_Pnt& thePnt,
					     const Standard_Real theParam,
					     const Standard_Boolean theIsReplace)
{
  return pnts.AddPoint(thePnt, theParam, theIsReplace);
}

Standard_Integer BRepMesh_GeomTool::NbPoints() const {
  return pnts.NbPoints();
}

void BRepMesh_GeomTool::Value(const Standard_Real IsoParam,
			       const Standard_Integer Index,
			       Standard_Real& W, gp_Pnt& P, gp_Pnt2d& UV) const
{
  P = pnts.Value(Index);
  W = pnts.Parameter(Index);
  if (parametric == GeomAbs_IsoU) {
    UV.SetCoord(IsoParam,W);
  }
  else {
    UV.SetCoord(W,IsoParam);
  }
}

void BRepMesh_GeomTool::Value(const BRepAdaptor_Curve& C,
			       const Handle(BRepAdaptor_HSurface)& S,
			       const Standard_Integer Index,
			       Standard_Real& W, gp_Pnt& P, gp_Pnt2d& UV) const
{
  P = pnts.Value(Index);
  W = pnts.Parameter(Index);
  BRepMesh_ShapeTool::Parameters(C.Edge(), ((BRepAdaptor_Surface*)&(S->Surface()))->Face(), W, UV);
}

void BRepMesh_GeomTool::D0(const Handle(BRepAdaptor_HSurface)& S,const Standard_Real U,
			    const Standard_Real V,gp_Pnt& P) 
{
  S->D0(U,V,P);
}  

Standard_Boolean BRepMesh_GeomTool::Normal(const Handle(BRepAdaptor_HSurface)& S,
					    const Standard_Real U,
					    const Standard_Real V, 
					    gp_Pnt& P,
					    gp_Dir& Nor) 
{
  Standard_Boolean OK = Standard_True;
  gp_Vec D1U,D1V;
  CSLib_DerivativeStatus Status;
  S->D1(U,V,P,D1U,D1V);
  CSLib::Normal(D1U,D1V,Standard_Real(Precision::Angular()),Status,Nor);
  if (Status!= CSLib_Done) {
    gp_Vec D2U,D2V,D2UV;
    S->D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
    CSLib_NormalStatus NStat;
    CSLib::Normal(D1U,D1V,D2U,D2V,D2UV,Precision::Angular(),OK,NStat,Nor);
  }
  if (OK) {
    if (BRepMesh_ShapeTool::Orientation(((BRepAdaptor_Surface*)&(S->Surface()))->Face()) == TopAbs_REVERSED)  Nor.Reverse();
  }
  return OK;
}
