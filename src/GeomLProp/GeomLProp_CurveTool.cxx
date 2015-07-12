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


#include <Geom_Curve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomLProp_CurveTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

void  GeomLProp_CurveTool::Value(const Handle(Geom_Curve)& C, 
	    const Standard_Real U, gp_Pnt& P)
{
  P = C->Value(U);
}

void  GeomLProp_CurveTool::D1(const Handle(Geom_Curve)& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1)
{
  C->D1(U, P, V1);
}

void  GeomLProp_CurveTool::D2(const Handle(Geom_Curve)& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2)
{
  C->D2(U, P, V1, V2);
}

void  GeomLProp_CurveTool::D3(const Handle(Geom_Curve)& C, 
	 const Standard_Real U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3)
{
  C->D3(U, P, V1, V2, V3);
}

Standard_Integer  GeomLProp_CurveTool::Continuity(const Handle(Geom_Curve)& C)
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

Standard_Real  GeomLProp_CurveTool::FirstParameter(const Handle(Geom_Curve)& C)
{
  return C->FirstParameter();
}

Standard_Real  GeomLProp_CurveTool::LastParameter(const Handle(Geom_Curve)& C)
{
  return C->LastParameter();
}



