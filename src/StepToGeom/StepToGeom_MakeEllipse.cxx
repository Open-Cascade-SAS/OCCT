// File:	StepToGeom_MakeEllipse.cxx
// Created:	Thu Sep  1 13:48:41 1994
// Author:	Frederic MAUPAS
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
