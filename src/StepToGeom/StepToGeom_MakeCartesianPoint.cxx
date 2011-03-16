// File:	StepToGeom_MakeCartesianPoint.cxx
// Created:	Fri Jul  2 16:09:46 1993
// Author:	Martine LANGLOIS

#include <StepToGeom_MakeCartesianPoint.ixx>
#include <StepGeom_CartesianPoint.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un CartesianPoint de Geom a partir d' un CartesianPoint de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeCartesianPoint::Convert
  (const Handle(StepGeom_CartesianPoint)& SP,
   Handle(Geom_CartesianPoint)& CP)
{
  if (SP->NbCoordinates() == 3)
  {
    const Standard_Real LF = UnitsMethods::LengthFactor();
    const Standard_Real X = SP->CoordinatesValue(1) * LF;
    const Standard_Real Y = SP->CoordinatesValue(2) * LF;
    const Standard_Real Z = SP->CoordinatesValue(3) * LF;
    CP = new Geom_CartesianPoint(X, Y, Z);
    return Standard_True;
  }
  return Standard_False;
}
