// Created on: 1992-08-18
// Created by: Herve LEGRAND
// Copyright (c) 1992-1999 Matra Datavision
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


#include <Geom2dLProp_Curve2dTool.ixx>
#include <Geom2d_Curve.hxx>
#include <GeomAbs_Shape.hxx>

void  Geom2dLProp_Curve2dTool::Value(const Handle_Geom2d_Curve& C, 
	    const Standard_Real U, gp_Pnt2d& P)
{
  P = C->Value(U);
}

void  Geom2dLProp_Curve2dTool::D1(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1)
{
  C->D1(U, P, V1);
}

void  Geom2dLProp_Curve2dTool::D2(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2)
{
  C->D2(U, P, V1, V2);
}

void  Geom2dLProp_Curve2dTool::D3(const Handle_Geom2d_Curve& C, 
	 const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2, gp_Vec2d& V3)
{
  C->D3(U, P, V1, V2, V3);
}

Standard_Integer  Geom2dLProp_Curve2dTool::Continuity(const Handle_Geom2d_Curve& C)
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

Standard_Real  Geom2dLProp_Curve2dTool::FirstParameter(const Handle_Geom2d_Curve& C)
{
  return C->FirstParameter();
}

Standard_Real  Geom2dLProp_Curve2dTool::LastParameter(const Handle_Geom2d_Curve& C)
{
  return C->LastParameter();
}




