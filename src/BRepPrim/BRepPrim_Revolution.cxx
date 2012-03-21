// Created on: 1992-11-06
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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




#include <BRepPrim_Revolution.ixx>

#include <Precision.hxx>
#include <Geom_SurfaceOfRevolution.hxx>

//=======================================================================
//function : BRepPrim_Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2& A, 
					 const Standard_Real VMin,
					 const Standard_Real VMax,
					 const Handle(Geom_Curve)& M,
					 const Handle(Geom2d_Curve)& PM) :
       BRepPrim_OneAxis(BRepPrim_Builder(),A,VMin,VMax),
       myMeridian(M),
       myPMeridian(PM)
{
}

//=======================================================================
//function : BRepPrim_Revolution
//purpose  : 
//=======================================================================

BRepPrim_Revolution::BRepPrim_Revolution(const gp_Ax2& A, 
					 const Standard_Real VMin,
					 const Standard_Real VMax) :
       BRepPrim_OneAxis(BRepPrim_Builder(),A,VMin,VMax)
{
}

//=======================================================================
//function : Meridian
//purpose  : 
//=======================================================================

void BRepPrim_Revolution::Meridian(const Handle(Geom_Curve)& M,
				   const Handle(Geom2d_Curve)& PM)
{
  myMeridian  = M;
  myPMeridian = PM;
}

//=======================================================================
//function : MakeEmptyLateralFace
//purpose  : 
//=======================================================================

TopoDS_Face  BRepPrim_Revolution::MakeEmptyLateralFace() const
{
  Handle(Geom_SurfaceOfRevolution) S =
    new Geom_SurfaceOfRevolution(myMeridian,Axes().Axis());

  TopoDS_Face F;
  myBuilder.Builder().MakeFace(F,S,Precision::Confusion());
  return F;
}

//=======================================================================
//function : MakeEmptyMeridianEdge
//purpose  : 
//=======================================================================

TopoDS_Edge  BRepPrim_Revolution::MakeEmptyMeridianEdge(const Standard_Real Ang) const 
{
  TopoDS_Edge E;
  Handle(Geom_Curve) C = Handle(Geom_Curve)::DownCast(myMeridian->Copy());
  gp_Trsf T;
  T.SetRotation(Axes().Axis(),Ang);
  C->Transform(T);
  myBuilder.Builder().MakeEdge(E,C,Precision::Confusion());
  return E;
}

//=======================================================================
//function : MeridianValue
//purpose  : 
//=======================================================================

gp_Pnt2d  BRepPrim_Revolution::MeridianValue(const Standard_Real V) const 
{
  return myPMeridian->Value(V);
}

//=======================================================================
//function : SetMeridianPCurve
//purpose  : 
//=======================================================================

void  BRepPrim_Revolution::SetMeridianPCurve(TopoDS_Edge& E, 
					     const TopoDS_Face& F) const 
{
  myBuilder.Builder().UpdateEdge(E,myPMeridian,F,Precision::Confusion());
}

