// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomGridEvaluator_BSplineSurface.hxx>

#include <BSplCLib.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

//==================================================================================================

void GeomGridEvaluator_BSplineSurface::Initialize(const Handle(Geom_BSplineSurface)& theSurface)
{
  myIsInitialized = false;

  if (theSurface.IsNull())
  {
    return;
  }

  // Extract poles (2D array)
  Handle(TColgp_HArray2OfPnt) aPoles =
    new TColgp_HArray2OfPnt(1, theSurface->NbUPoles(), 1, theSurface->NbVPoles());
  theSurface->Poles(aPoles->ChangeArray2());

  // Extract weights if rational
  Handle(TColStd_HArray2OfReal) aWeights;
  if (theSurface->IsURational() || theSurface->IsVRational())
  {
    aWeights = new TColStd_HArray2OfReal(1, theSurface->NbUPoles(), 1, theSurface->NbVPoles());
    theSurface->Weights(aWeights->ChangeArray2());
  }

  // Build flat knot sequences
  const int aUDegree = theSurface->UDegree();
  const int aVDegree = theSurface->VDegree();

  const int aUFlatLength = BSplCLib::KnotSequenceLength(theSurface->UMultiplicities(),
                                                         aUDegree,
                                                         theSurface->IsUPeriodic());
  Handle(TColStd_HArray1OfReal) aUFlatKnots = new TColStd_HArray1OfReal(1, aUFlatLength);
  BSplCLib::KnotSequence(theSurface->UKnots(),
                         theSurface->UMultiplicities(),
                         aUDegree,
                         theSurface->IsUPeriodic(),
                         aUFlatKnots->ChangeArray1());

  const int aVFlatLength = BSplCLib::KnotSequenceLength(theSurface->VMultiplicities(),
                                                         aVDegree,
                                                         theSurface->IsVPeriodic());
  Handle(TColStd_HArray1OfReal) aVFlatKnots = new TColStd_HArray1OfReal(1, aVFlatLength);
  BSplCLib::KnotSequence(theSurface->VKnots(),
                         theSurface->VMultiplicities(),
                         aVDegree,
                         theSurface->IsVPeriodic(),
                         aVFlatKnots->ChangeArray1());

  myIsInitialized = myEvaluator.Initialize(aUDegree,
                                           aVDegree,
                                           aPoles,
                                           aWeights,
                                           aUFlatKnots,
                                           aVFlatKnots,
                                           theSurface->IsURational(),
                                           theSurface->IsVRational(),
                                           theSurface->IsUPeriodic(),
                                           theSurface->IsVPeriodic());
}
