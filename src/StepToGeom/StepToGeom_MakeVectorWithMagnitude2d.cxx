// File:	StepToGeom_MakeVectorWithMagnitude2d.cxx
// Created:	Wed Aug  4 11:45:12 1993
// Author:	Martine LANGLOIS
// sln 23.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude2d(...) function)

#include <StepToGeom_MakeVectorWithMagnitude2d.ixx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_Direction.hxx>
#include <StepToGeom_MakeDirection2d.hxx>
#include <Geom2d_Direction.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>

//=============================================================================
// Creation d' un VectorWithMagnitude de Geom2d a partir d' un Vector de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeVectorWithMagnitude2d::Convert (const Handle(StepGeom_Vector)& SV, Handle(Geom2d_VectorWithMagnitude)& CV)
{
  // sln 23.10.2001. CTS23496: Vector is not created if direction have not been succesfully created (StepToGeom_MakeVectorWithMagnitude2d(...) function)
  Handle(Geom2d_Direction) D;
  if (StepToGeom_MakeDirection2d::Convert(SV->Orientation(),D))
  {
    const gp_Vec2d V(D->Dir2d().XY() * SV->Magnitude());
    CV = new Geom2d_VectorWithMagnitude(V);
    return Standard_True;
  }
  return Standard_False;
}
