// Created on: 1999-02-16
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

// sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used (StepToGeom_MakeTransformation3d(...) function)

#include <StepToGeom_MakeTransformation3d.ixx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Direction.hxx>
#include <gp_Ax3.hxx>

//=======================================================================
//function : StepToGeom_MakeTransformation3d
//purpose  : 
//=======================================================================

Standard_Boolean StepToGeom_MakeTransformation3d::Convert (const Handle(StepGeom_CartesianTransformationOperator3d)& SCTO, gp_Trsf& CT)
{
  Handle(Geom_CartesianPoint) CP;
  if (StepToGeom_MakeCartesianPoint::Convert(SCTO->LocalOrigin(),CP))
  {
    const gp_Pnt Pgp = CP->Pnt();

    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    gp_Dir D1(1.,0.,0.);
    const Handle(StepGeom_Direction) A1 = SCTO->Axis1();
    if (!A1.IsNull()) {
      Handle(Geom_Direction) D;
      if (StepToGeom_MakeDirection::Convert(A1,D))
        D1 = D->Dir();
    }

    gp_Dir D2(0.,1.,0.);
    const Handle(StepGeom_Direction) A2 = SCTO->Axis2();
    if (!A2.IsNull()) {
      Handle(Geom_Direction) D;
      if (StepToGeom_MakeDirection::Convert(A2,D))
        D2 = D->Dir();
    }

    Standard_Boolean isDefaultDirectionUsed = Standard_True;
    gp_Dir D3;
    const Handle(StepGeom_Direction) A3 = SCTO->Axis3();
    if (!A3.IsNull()) {
      Handle(Geom_Direction) D;
	  if(StepToGeom_MakeDirection::Convert(A3,D)) {
        D3 = D->Dir();
        isDefaultDirectionUsed = Standard_False;
      }
    }
    if(isDefaultDirectionUsed) 
      D3 = D1.Crossed(D2);

    const gp_Ax3 result(Pgp,D3,D1);
    CT.SetTransformation(result);
    CT = CT.Inverted(); //:n8 abv 16 Feb 99: tr8_as2_db.stp: reverse for accordance with LV tool
    return Standard_True;
  }
  return Standard_False;
}
