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

//:o9 abv 19.02.99: bm4_id_punch_b.stp #678: treatment of minR > maxR
//                  NOTE: parametrisation of resulting ellipse shifted on pi/2

#include <StepToGeom_MakeEllipse2d.ixx>
#include <StepGeom_Ellipse.hxx>
#include <StepGeom_Axis2Placement2d.hxx>
#include <Geom2d_AxisPlacement.hxx>
#include <StepToGeom_MakeAxisPlacement.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax22d.hxx>

//=============================================================================
// Creation d' un Ellipse de Geom2d a partir d' un Ellipse de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeEllipse2d::Convert
    (const Handle(StepGeom_Ellipse)& SC,
     Handle(Geom2d_Ellipse)& CC)
{
  const StepGeom_Axis2Placement AxisSelect = SC->Position();
  if (AxisSelect.CaseNum(AxisSelect.Value()) == 1) {
    Handle(Geom2d_AxisPlacement) A1;
    if (StepToGeom_MakeAxisPlacement::Convert
          (Handle(StepGeom_Axis2Placement2d)::DownCast(AxisSelect.Value()),A1))
    {
      gp_Ax22d A( A1->Ax2d() );
      const Standard_Real majorR = SC->SemiAxis1();
      const Standard_Real minorR = SC->SemiAxis2();
      if ( majorR - minorR >= 0. ) { //:o9 abv 19 Feb 99: bm4_id_punch_b.stp #678: protection
        CC = new Geom2d_Ellipse(A, majorR, minorR);
      }
      else {
        const gp_Dir2d X = A.XDirection();
        A.SetXDirection ( gp_Dir2d ( X.X(), -X.Y() ) );
        CC = new Geom2d_Ellipse(A, minorR, majorR);
      }
      return Standard_True;
	}
  }
  return Standard_False;
}
