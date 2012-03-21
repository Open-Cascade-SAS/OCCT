// Created on: 1994-09-01
// Created by: Frederic MAUPAS
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

//:o9 abv 19.02.99: bm4_id_punch_b.stp #678: treatment of ellipse with minR > maxR
//                  NOTE: parametrisation of resulting ellipse shifted on pi/2

#include <StepToGeom_MakeEllipse.ixx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <Geom_Axis2Placement.hxx>
#include <StepToGeom_MakeAxis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un Ellipse de Geom a partir d' un Ellipse de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeEllipse::Convert
    (const Handle(StepGeom_Ellipse)& SC,
     Handle(Geom_Ellipse)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 2) {
    Handle(Geom_Axis2Placement) A1;
    if (StepToGeom_MakeAxis2Placement::Convert
          (Handle(StepGeom_Axis2Placement3d)::DownCast(AxisSelect.Value()),A1))
    {
      gp_Ax2 A( A1->Ax2() );
      const Standard_Real LF = UnitsMethods::LengthFactor();
      const Standard_Real majorR = SC->SemiAxis1() * LF;
      const Standard_Real minorR = SC->SemiAxis2() * LF;
      if ( majorR - minorR >= 0. ) { //:o9 abv 19 Feb 99
        CC = new Geom_Ellipse(A, majorR, minorR);
      }
      //:o9 abv 19 Feb 99
      else {
        A.SetXDirection ( A.XDirection() ^ A.Direction() );
        CC = new Geom_Ellipse(A, minorR, majorR);
      }
      return Standard_True;
    }
  }
  return Standard_False;
}
