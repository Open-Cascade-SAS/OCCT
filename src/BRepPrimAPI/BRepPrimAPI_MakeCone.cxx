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
#include <BRepPrim_Cone.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrimAPI_MakeCone
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const Standard_Real R1,
				   const Standard_Real R2, 
				   const Standard_Real H) :
       myCone(gp::XOY(),R1, R2, H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCone
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const Standard_Real R1,
				   const Standard_Real R2,
				   const Standard_Real H,
				   const Standard_Real angle) :
       myCone( R1, R2, H)
{
  myCone.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeCone
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const gp_Ax2& Axes,
				   const Standard_Real R1, 
				   const Standard_Real R2,
				   const Standard_Real H) :
       myCone( Axes, R1, R2, H)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeCone
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCone::BRepPrimAPI_MakeCone(const gp_Ax2& Axes,
				   const Standard_Real R1,
				   const Standard_Real R2,
				   const Standard_Real H,
				   const Standard_Real angle) :
       myCone( Axes, R1, R2, H)
{
  myCone.Angle(angle);
}


//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeCone::OneAxis()
{
  return &myCone;
}


//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

BRepPrim_Cone&  BRepPrimAPI_MakeCone::Cone()
{
  return myCone;
}



