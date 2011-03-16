// File:	StepToGeom_MakeDirection.cxx
// Created:	Fri Jul  2 17:32:04 1993
// Author:	Martine LANGLOIS
// sln 22.10.2001. CTS23496: Direction is not created if it has null magnitude (StepToGeom_MakeDirection(...) function)

#include <StepToGeom_MakeDirection.ixx>
#include <StepGeom_Direction.hxx>

//=============================================================================
// Creation d' un Direction de Geom a partir d' un Direction de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeDirection::Convert (const Handle(StepGeom_Direction)& SD, Handle(Geom_Direction)& CD)
{
  if (SD->NbDirectionRatios() >= 3)
  {
    const Standard_Real X = SD->DirectionRatiosValue(1);
    const Standard_Real Y = SD->DirectionRatiosValue(2);
    const Standard_Real Z = SD->DirectionRatiosValue(3);
    // sln 22.10.2001. CTS23496: Direction is not created if it has null magnitude
    if (gp_XYZ(X, Y, Z).SquareModulus() > gp::Resolution()*gp::Resolution())
    {
      CD = new Geom_Direction(X, Y, Z);
      return Standard_True;
    }
  }
  return Standard_False;
}
