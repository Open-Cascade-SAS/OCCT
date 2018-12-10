// Created on: 1993-06-25
// Created by: Laurent BOURESCHE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRepSweep_Revol.hxx>
#include <BRepSweep_Rotation.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Sweep_NumShape.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>

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


//=======================================================================
//function : IsUsed
//purpose  : 
//=======================================================================
Standard_Boolean BRepSweep_Revol::IsUsed(const TopoDS_Shape& aGenS) const
{
  return myRotation.IsUsed(aGenS);
}
