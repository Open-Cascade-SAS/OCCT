// Created on: 1994-02-24
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <LProp3d_CurveTool.ixx>

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


