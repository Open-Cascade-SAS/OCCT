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
#include <BRepPrim_Sphere.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DomainError.hxx>

//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R) :
       mySphere(gp::XOY(),R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R, 
				       const Standard_Real angle) :
       mySphere(gp_Ax2(gp::Origin(), (angle<0.?-1:1)*gp::DZ(), gp::DX()),
		R)
{
  mySphere.Angle(Abs(angle));
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2) :
       mySphere(R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const Standard_Real R, 
				       const Standard_Real angle1,
				       const Standard_Real angle2, 
				       const Standard_Real angle3) :
       mySphere(R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}

//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R, 
				       const Standard_Real angle) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R,
				       const Standard_Real angle1,
				       const Standard_Real angle2) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Pnt& Center,
				       const Standard_Real R,
				       const Standard_Real angle1,
				       const Standard_Real angle2,
				       const Standard_Real angle3) :
       mySphere(gp_Ax2(Center, gp_Dir(0,0,1), gp_Dir(1,0,0)),
		R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis,
				       const Standard_Real R) :
       mySphere(Axis, R)
{
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis,
				       const Standard_Real R,
				       const Standard_Real angle) :
       mySphere( Axis, R)
{
  mySphere.Angle(angle);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis, 
				       const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2) :
       mySphere(Axis, R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
}


//=======================================================================
//function : BRepPrimAPI_MakeSphere
//purpose  : 
//=======================================================================

BRepPrimAPI_MakeSphere::BRepPrimAPI_MakeSphere(const gp_Ax2& Axis, 
				       const Standard_Real R,
				       const Standard_Real angle1, 
				       const Standard_Real angle2, 
				       const Standard_Real angle3) :
       mySphere( Axis, R)
{
  mySphere.VMin(angle1);
  mySphere.VMax(angle2);
  mySphere.Angle(angle3);
}



//=======================================================================
//function : OneAxis
//purpose  : 
//=======================================================================

Standard_Address  BRepPrimAPI_MakeSphere::OneAxis()
{
  return &mySphere;
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

BRepPrim_Sphere&  BRepPrimAPI_MakeSphere::Sphere()
{
  return mySphere;
}


