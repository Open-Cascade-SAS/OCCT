// File:	StepToGeom_MakeCartesianPoint2d.cxx
// Created:	Fri Jul  2 16:21:08 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeCartesianPoint2d.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>

//=============================================================================
// Creation d' un CartesianPoint de Geom2d a partir d' un CartesianPoint de
// Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCartesianPoint2d::Convert
  (const Handle(StepGeom_CartesianPoint)& SP,
   Handle(Geom2d_CartesianPoint)& CP)
{
  if (SP->NbCoordinates() == 2)
  {
    const Standard_Real X = SP->CoordinatesValue(1);
    const Standard_Real Y = SP->CoordinatesValue(2);
    CP = new Geom2d_CartesianPoint(X, Y);
    return Standard_True;
  }
  return Standard_False;
}
