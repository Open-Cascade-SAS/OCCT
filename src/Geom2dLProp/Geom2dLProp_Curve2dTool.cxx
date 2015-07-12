// Created on: 1992-08-18
// Created by: Herve LEGRAND
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Geom2dLProp_Curve2dTool.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

void  Geom2dLProp_Curve2dTool::Value(const Handle(Geom2d_Curve)& C, 
	    const Standard_Real U, gp_Pnt2d& P)
{
  P = C->Value(U);
}

void  Geom2dLProp_Curve2dTool::D1(const Handle(Geom2d_Curve)& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1)
{
  C->D1(U, P, V1);
}

void  Geom2dLProp_Curve2dTool::D2(const Handle(Geom2d_Curve)& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2)
{
  C->D2(U, P, V1, V2);
}

void  Geom2dLProp_Curve2dTool::D3(const Handle(Geom2d_Curve)& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3)
{
  C->D3(U, P, V1, V2, V3);
}

Standard_Integer  Geom2dLProp_Curve2dTool::Continuity(const Handle(Geom2d_Curve)& C)
{
  GeomAbs_Shape s = C->Continuity();
  switch (s) {
  case GeomAbs_C0:
    return 0;
  case GeomAbs_C1:
    return 1;
  case GeomAbs_C2:
    return 2;
  case GeomAbs_C3:
    return 3;
  case GeomAbs_G1:
    return 0;
  case GeomAbs_G2:
    return 0;
  case GeomAbs_CN:
    return 3;
  };
  return 0;
}

Standard_Real  Geom2dLProp_Curve2dTool::FirstParameter(const Handle(Geom2d_Curve)& C)
{
  return C->FirstParameter();
}

Standard_Real  Geom2dLProp_Curve2dTool::LastParameter(const Handle(Geom2d_Curve)& C)
{
  return C->LastParameter();
}




