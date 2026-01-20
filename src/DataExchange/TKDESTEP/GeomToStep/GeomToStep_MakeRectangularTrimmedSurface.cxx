// Created on: 1996-01-25
// Created by: Frederic MAUPAS
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomToStep_MakeRectangularTrimmedSurface.hxx>
#include <GeomToStep_MakeSurface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepGeom_RectangularTrimmedSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une rectangular_trimmed_surface de STEP
// a partir d' une RectangularTrimmedSurface de Geom
//=============================================================================
GeomToStep_MakeRectangularTrimmedSurface::GeomToStep_MakeRectangularTrimmedSurface(
  const occ::handle<Geom_RectangularTrimmedSurface>& RTSurf,
  const StepData_Factors&                       theLocalFactors)
{

  occ::handle<StepGeom_RectangularTrimmedSurface> StepRTS = new StepGeom_RectangularTrimmedSurface;

  occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");

  GeomToStep_MakeSurface mkSurf(RTSurf->BasisSurface(), theLocalFactors);
  if (!mkSurf.IsDone())
  {
    done = false;
    return;
  }
  const occ::handle<StepGeom_Surface>& StepSurf = mkSurf.Value();

  double U1, U2, V1, V2;
  RTSurf->Bounds(U1, U2, V1, V2);

  // -----------------------------------------
  // Modification of the Trimming Parameters ?
  // -----------------------------------------

  double        AngleFact  = 180. / M_PI;
  double        uFact      = 1.;
  double        vFact      = 1.;
  double        LengthFact = theLocalFactors.LengthFactor();
  occ::handle<Geom_Surface> theSurf    = RTSurf->BasisSurface();
  if (theSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
  {
    uFact = AngleFact;
    vFact = 1. / LengthFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    uFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))
           || theSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    uFact = AngleFact;
    vFact = AngleFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
  {
    occ::handle<Geom_ConicalSurface> conicS = occ::down_cast<Geom_ConicalSurface>(theSurf);
    double               semAng = conicS->SemiAngle();
    uFact                              = AngleFact;
    vFact                              = std::cos(semAng) / LengthFact;
  }
  else if (theSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    uFact = vFact = 1. / LengthFact;
  }

  U1 = U1 * uFact;
  U2 = U2 * uFact;
  V1 = V1 * vFact;
  V2 = V2 * vFact;

  StepRTS->Init(aName, StepSurf, U1, U2, V1, V2, true, true);
  theRectangularTrimmedSurface = StepRTS;
  done                         = true;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const occ::handle<StepGeom_RectangularTrimmedSurface>& GeomToStep_MakeRectangularTrimmedSurface::Value()
  const
{
  StdFail_NotDone_Raise_if(!done, "GeomToStep_MakeRectangularTrimmedSurface::Value() - no result");
  return theRectangularTrimmedSurface;
}
