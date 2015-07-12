// Created on: 1993-07-23
// Created by: Remi LEQUETTE
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


#include <BRepBuilderAPI.hxx>
#include <BRepPrim_Torus.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <gp_Ax2.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================
BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2, 
				     const Standard_Real angle1, 
				     const Standard_Real angle2) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle1, 
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus( R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2) :
       myTorus(Axes, R1, R2)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1,
				     const Standard_Real R2,
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes, 
				     const Standard_Real R1, 
				     const Standard_Real R2, 
				     const Standard_Real angle1,
				     const Standard_Real angle2) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeTorus
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeTorus::BRepPrimAPI_MakeTorus(const gp_Ax2& Axes,
				     const Standard_Real R1, 
				     const Standard_Real R2,
				     const Standard_Real angle1,
				     const Standard_Real angle2, 
				     const Standard_Real angle) :
       myTorus(Axes, R1, R2)
{
  myTorus.VMin(angle1);
  myTorus.VMax(angle2);
  myTorus.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeTorus::OneAxis()
{
  return &myTorus;
}


//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

BRepPrim_Torus&  BRepPrimAPI_MakeTorus::Torus()
{
  return myTorus;
}


