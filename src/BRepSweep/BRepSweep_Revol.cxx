// Created on: 1993-06-25
// Created by: Laurent BOURESCHE
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



#include <BRepSweep_Revol.ixx>
#include <BRepSweep_Rotation.hxx>
#include <Sweep_NumShape.hxx>

#include <gp_Trsf.hxx>
#include <Precision.hxx>
//=======================================================================
//function : BRepSweep_Revol
//purpose  : 
//=======================================================================

BRepSweep_Revol::BRepSweep_Revol
  (const TopoDS_Shape& S, 
   const gp_Ax1& Ax, 
   const Standard_Real D,
   const Standard_Boolean C):
  myRotation(S.Oriented(TopAbs_FORWARD),
	     NumShape(D),
	     Location(Ax,D),
	     Axe(Ax,D),
	     Angle(D),
	     C)
{
  Standard_ConstructionError_Raise_if
    (Angle(D)<=Precision::Angular(),"BRepSweep_Revol::Constructor");
}

//=======================================================================
//function : BRepSweep_Revol
//purpose  : 
//=======================================================================

BRepSweep_Revol::BRepSweep_Revol
  (const TopoDS_Shape& S, 
   const gp_Ax1& Ax, 
   const Standard_Boolean C):
  myRotation(S.Oriented(TopAbs_FORWARD),
	     NumShape(2*M_PI),
	     Location(Ax,2*M_PI),
	     Axe(Ax,2*M_PI),
	     Angle(2*M_PI),
	     C)

{
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::Shape()
{
  return myRotation.Shape();
}


//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::Shape(const TopoDS_Shape& aGenS)
{
  return myRotation.Shape(aGenS);
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::FirstShape()
{
  return myRotation.FirstShape();
}


//=======================================================================
//function : FirstShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::FirstShape(const TopoDS_Shape& aGenS)
{
  return myRotation.FirstShape(aGenS);
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::LastShape()
{
  return myRotation.LastShape();
}


//=======================================================================
//function : LastShape
//purpose  : 
//=======================================================================

TopoDS_Shape  BRepSweep_Revol::LastShape(const TopoDS_Shape& aGenS)
{
  return myRotation.LastShape(aGenS);
}


//=======================================================================
//function : NumShape
//purpose  : 
//=======================================================================

Sweep_NumShape  BRepSweep_Revol::NumShape(const Standard_Real D)const 
{
  Sweep_NumShape N;
  if (Abs(Angle(D) - 2*M_PI)<=Precision::Angular()){
    N.Init(2,TopAbs_EDGE,Standard_True,
	   Standard_False,Standard_False);
  }
  else{
    N.Init(2,TopAbs_EDGE);
  }
  return N;
}


//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

TopLoc_Location  BRepSweep_Revol::Location(const gp_Ax1& Ax, 
					   const Standard_Real D)const 
{
  gp_Trsf gpt;
  gpt.SetRotation(Axe(Ax,D),Angle(D));
  TopLoc_Location L(gpt);
  return L;
}


//=======================================================================
//function : Axe
//purpose  : 
//=======================================================================

gp_Ax1  BRepSweep_Revol::Axe(const gp_Ax1& Ax, const Standard_Real D)const 
{
  gp_Ax1 A = Ax;
  if ( D < 0. ) A.Reverse();
  return A;
}


//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real  BRepSweep_Revol::Angle(const Standard_Real D)const 
{
  Standard_Real d = Abs(D);
  while(d>(2*M_PI + Precision::Angular())){
    d = d - 2*M_PI;
  }
  return d;
}


//=======================================================================
//function : Angle
//purpose  : 
//=======================================================================

Standard_Real  BRepSweep_Revol::Angle()const 
{
  return myRotation.Angle();
}


//=======================================================================
//function : Axe
//purpose  : 
//=======================================================================

gp_Ax1  BRepSweep_Revol::Axe()const 
{
  return myRotation.Axe();
}


