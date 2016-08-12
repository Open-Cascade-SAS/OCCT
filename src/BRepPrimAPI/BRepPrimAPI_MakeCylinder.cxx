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
#include <BRepPrim_Cylinder.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const Standard_Real R,
					   const Standard_Real H) :
       myCylinder(gp::XOY(), R , H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const Standard_Real R, 
					   const Standard_Real H, 
					   const Standard_Real Angle) :
       myCylinder( R , H)
{
  myCylinder.Angle(Angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes, 
					   const Standard_Real R, 
					   const Standard_Real H) :
       myCylinder( Axes, R , H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const gp_Ax2& Axes,
					   const Standard_Real R, 
					   const Standard_Real H,
					   const Standard_Real Angle) :
       myCylinder( Axes, R , H)
{
  myCylinder.Angle(Angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeCylinder::OneAxis()
{
  return &myCylinder;
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

BRepPrim_Cylinder&  BRepPrimAPI_MakeCylinder::Cylinder()
{
  return myCylinder;
}



