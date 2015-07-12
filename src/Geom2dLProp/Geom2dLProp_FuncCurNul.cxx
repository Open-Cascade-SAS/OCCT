// Created on: 1994-09-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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
#include <Geom2dLProp_FuncCurNul.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

//=============================================================================
//function :
// purpose :
//=============================================================================
Geom2dLProp_FuncCurNul::Geom2dLProp_FuncCurNul(const Handle(Geom2d_Curve)& C)
:theCurve(C)
{
}

//=============================================================================
//function : Value 
// purpose : F = (V1^V2.Z)/||V1||*||V2||
//=============================================================================
Standard_Boolean Geom2dLProp_FuncCurNul::Value (const Standard_Real  X,
					        Standard_Real& F)
{
  Standard_Real D;
  return Values(X,F,D);
}

//=============================================================================
//function : Derivative
// purpose :
//=============================================================================
Standard_Boolean Geom2dLProp_FuncCurNul::Derivative(const Standard_Real  X,
			                            Standard_Real& D)
{  
  Standard_Real F;
  return Values(X,F,D);
}

//=============================================================================
//function : Values
// purpose : F = (V1^V2.Z)/||V1||*||V2||
//=============================================================================
Standard_Boolean Geom2dLProp_FuncCurNul::Values (const Standard_Real  X,
					         Standard_Real& F,
					         Standard_Real& D)
{
  gp_Pnt2d P1;
  gp_Vec2d V1,V2,V3;
  Geom2dLProp_Curve2dTool::D3(theCurve,X,P1,V1,V2,V3);
  Standard_Real CP1  = V1.Crossed(V2);
  Standard_Real CP2  = V1.Crossed(V3);
  Standard_Real V1V2 = V1.Dot(V2);
  Standard_Real V2V3 = V2.Dot(V3);
  Standard_Real NV1  = V1.Magnitude();
  Standard_Real NV2  = V2.Magnitude();

  F = 0. ;
  D = 0. ;

/*
  if (Abs(CP1) < 1.e-4) {
    return Standard_True;
  } else */

  if (NV2 < 1.e-4) {
    return Standard_True;
  } else if (NV1*NV2 < gp::Resolution()) {
    return Standard_False;
  } else {
    F   = CP1/(NV1*NV2);
    D   = (CP2 - CP1*V1V2/(NV1*NV1) - CP1*V2V3/(NV2*NV2))/(NV1*NV2);
  }
  return Standard_True;
  
}



