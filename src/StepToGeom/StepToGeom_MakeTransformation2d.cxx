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

// sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used (StepToGeom_MakeTransformation2d(...) function)

#include <StepToGeom_MakeTransformation2d.ixx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <StepToGeom_MakeDirection2d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Direction.hxx>
#include <gp_Ax2d.hxx>

//=======================================================================
//function : StepToGeom_MakeTransformation2d
//purpose  : 
//=======================================================================

Standard_Boolean StepToGeom_MakeTransformation2d::Convert (const Handle(StepGeom_CartesianTransformationOperator2d)& SCTO, gp_Trsf2d& CT)
{
  //  NB : on ne s interesse ici qu au deplacement rigide
  Handle(Geom2d_CartesianPoint) CP;
  if (StepToGeom_MakeCartesianPoint2d::Convert(SCTO->LocalOrigin(),CP))
  {
    gp_Dir2d D1(1.,0.);
    // sln 23.10.2001. CTS23496: If problems with creation of direction occur default direction is used
    const Handle(StepGeom_Direction) A = SCTO->Axis1();
    if (!A.IsNull())
    {
      Handle(Geom2d_Direction) D;
      if (StepToGeom_MakeDirection2d::Convert(A,D))
        D1 = D->Dir2d();
    }
    const gp_Ax2d result(CP->Pnt2d(),D1);
    CT.SetTransformation(result);
    CT = CT.Inverted();
    return Standard_True;
  }
  return Standard_False;
}
