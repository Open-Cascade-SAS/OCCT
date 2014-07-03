// Created on: 1993-07-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created (StepToGeom_MakeSurfaceOfLinearExtrusion(...) function)

#include <StepToGeom_MakeSurfaceOfLinearExtrusion.ixx>
#include <StepGeom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_VectorWithMagnitude.hxx>
#include <StepToGeom_MakeVectorWithMagnitude.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <StepToGeom_MakeCurve.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <Precision.hxx>

//=============================================================================
// Creation d' une SurfaceOfLinearExtrusion de Geom a partir d' une
// SurfaceOfLinearExtrusion de Step
//=============================================================================

Standard_Boolean StepToGeom_MakeSurfaceOfLinearExtrusion::Convert (const Handle(StepGeom_SurfaceOfLinearExtrusion)& SS,
                                                                   Handle(Geom_SurfaceOfLinearExtrusion)& CS)
{
  Handle(Geom_Curve) C;
  if (StepToGeom_MakeCurve::Convert(SS->SweptCurve(),C))
  {
    // sln 23.10.2001. CTS23496: Surface is not created if extrusion axis have not been succesfully created
    Handle(Geom_VectorWithMagnitude) V;
    if (StepToGeom_MakeVectorWithMagnitude::Convert(SS->ExtrusionAxis(),V))
    {
      const gp_Dir D(V->Vec());
      Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(C);
      if (!aLine.IsNull() && aLine->Lin().Direction().IsParallel(D, Precision::Angular()))
        return Standard_False;
      CS = new Geom_SurfaceOfLinearExtrusion(C,D);
      return Standard_True;
    }
  }
  return Standard_False;
}
