// File:	StepToGeom_MakeLine2d.cxx
// Created:	Wed Aug  4 10:55:40 1993
// Author:	Martine LANGLOIS
// sln 23.10.2001. CTS23496: Line is not created if direction have not been succesfully created (StepToGeom_MakeLine2d(...) function)

#include <StepToGeom_MakeLine2d.ixx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeVectorWithMagnitude2d.hxx>
#include <StepToGeom_MakeCartesianPoint2d.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Vector.hxx>
#include <Geom2d_VectorWithMagnitude.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>

//=============================================================================
// Creation d' une Line de Geom2d a partir d' une Line de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeLine2d::Convert (const Handle(StepGeom_Line)& SC, Handle(Geom2d_Line)& CC)
{
  Handle(Geom2d_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint2d::Convert(SC->Pnt(),P))
  {
    // sln 23.10.2001. CTS23496: Line is not created if direction have not been succesfully created
    Handle(Geom2d_VectorWithMagnitude) D;
    if (StepToGeom_MakeVectorWithMagnitude2d::Convert(SC->Dir(),D))
    {
      const gp_Dir2d D1(D->Vec2d());
      CC = new Geom2d_Line(P->Pnt2d(), D1);
      return Standard_True;
    }
  }
  return Standard_False;
}
