// File:	StepToGeom_MakeLine.cxx
// Created:	Fri Jul  2 17:51:47 1993
// Author:	Martine LANGLOIS
// sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created (StepToGeom_MakeLine(...) function)

#include <StepToGeom_MakeLine.ixx>
#include <StepGeom_Line.hxx>
#include <StepGeom_Vector.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <StepToGeom_MakeCartesianPoint.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

//=============================================================================
// Creation d' une Line de Geom a partir d' une Line de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeLine::Convert (const Handle(StepGeom_Line)& SC, Handle(Geom_Line)& CC)
{
  Handle(Geom_CartesianPoint) P;
  if (StepToGeom_MakeCartesianPoint::Convert(SC->Pnt(),P))
  {
    // sln 22.10.2001. CTS23496: Line is not created if direction have not been succesfully created 
    Handle(Geom_VectorWithMagnitude) D;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SC->Dir(),D))
    {
      const gp_Dir V(D->Vec());
      CC = new Geom_Line(P->Pnt(), V);
      return Standard_True;
    }
  }
  return Standard_False;
}
