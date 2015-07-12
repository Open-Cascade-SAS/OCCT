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

static gp_Ax2 CylinderComputeAxes() { 
  static Standard_Integer firsttime=1;
  static Standard_Integer modif=0;
  static Standard_Real cosa=cos(0.1);
  static Standard_Real sina=sin(0.1);
  static Standard_Real ux=1.0;
  static Standard_Real uy=0.0;
  
  if(firsttime) { 
    modif = getenv("PRIM_CYLINDER") != NULL;
    firsttime = 0;
  }
  if(modif) { 
    Standard_Real nux = cosa*ux+sina*uy;
    Standard_Real nuy =-sina*ux+cosa*uy;
    ux=nux; uy=nuy;
    return(gp_Ax2(gp::Origin(),gp::DZ(),gp_Dir(ux,uy,0.0)));
  }
  else { 
    return(gp::XOY());
  }

}



//=======================================================================
//function : BRepPrimAPI_MakeCylinder
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeCylinder::BRepPrimAPI_MakeCylinder(const Standard_Real R,
					   const Standard_Real H) :
       myCylinder(CylinderComputeAxes(), R , H)
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



