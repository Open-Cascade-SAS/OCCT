// File:	StepToGeom_MakeVectorWithMagnitude.cxx
// Created:	Mon Jul  5 11:49:43 1993
// Author:	Martine LANGLOIS
// sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude(...) function)

#include <StepToGeom_MakeVectorWithMagnitude.ixx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Direction.hxx>
#include <StepToGeom_MakeDirection.hxx>
#include <Geom_Direction.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <UnitsMethods.hxx>

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom a partir d' un Vector de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeVectorWithMagnitude::Convert (const Handle(StepGeom_Vector)& SV, Handle(Geom_VectorWithMagnitude)& CV)
{
  // sln 22.10.2001. CTS23496: Vector is not created if direction have not been succesfully created 
  Handle(Geom_Direction) D;
  if (StepToGeom_MakeDirection::Convert(SV->Orientation(),D))
  {
    const gp_Vec V(D->Dir().XYZ() * SV->Magnitude() * UnitsMethods::LengthFactor());
    CV = new Geom_VectorWithMagnitude(V);
    return Standard_True;
  }
  return Standard_False;
}
