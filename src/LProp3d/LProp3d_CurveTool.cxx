// Created on: 1994-02-24
// Created by: Laurent BOURESCHE
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


#include <Adaptor3d_HCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <LProp3d_CurveTool.hxx>

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
void LProp3d_CurveTool::Value(const Handle(Adaptor3d_HCurve)& C, 
                              const Standard_Real U, 
                              gp_Pnt& P)
{
  P = C->Value(U);
}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void LProp3d_CurveTool::D1(const Handle(Adaptor3d_HCurve)& C, 
                           const Standard_Real U, 
                           gp_Pnt& P, 
                           gp_Vec& V1)
{
  C->D1(U,P,V1);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void LProp3d_CurveTool::D2(const Handle(Adaptor3d_HCurve)& C, 
                           const Standard_Real U, 
                           gp_Pnt& P, 
                           gp_Vec& V1, 
                           gp_Vec& V2)
{
  C->D2(U,P,V1,V2);
}


//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void LProp3d_CurveTool::D3(const Handle(Adaptor3d_HCurve)& C, 
                           const Standard_Real U, 
                           gp_Pnt& P, 
                           gp_Vec& V1, 
                           gp_Vec& V2, 
                           gp_Vec& V3)
{
  C->D3(U,P,V1,V2,V3);
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

Standard_Integer LProp3d_CurveTool::Continuity
  (const Handle(Adaptor3d_HCurve)& C)
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


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real LProp3d_CurveTool::FirstParameter
  (const Handle(Adaptor3d_HCurve)& C)
{
  return C->FirstParameter();
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real LProp3d_CurveTool::LastParameter
  (const Handle(Adaptor3d_HCurve)& C)
{
  return C->LastParameter();
}


