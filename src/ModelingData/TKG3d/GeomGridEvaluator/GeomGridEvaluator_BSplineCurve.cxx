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

#include <GeomGridEvaluator_BSplineCurve.hxx>

#include <BSplCLib.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

//==================================================================================================

void GeomGridEvaluator_BSplineCurve::Initialize(const Handle(Geom_BSplineCurve)& theCurve)
{
  myIsInitialized = false;

  if (theCurve.IsNull())
  {
    return;
  }

  // Extract poles
  Handle(TColgp_HArray1OfPnt) aPoles = new TColgp_HArray1OfPnt(1, theCurve->NbPoles());
  theCurve->Poles(aPoles->ChangeArray1());

  // Extract weights if rational
  Handle(TColStd_HArray1OfReal) aWeights;
  if (theCurve->IsRational())
  {
    aWeights = new TColStd_HArray1OfReal(1, theCurve->NbPoles());
    theCurve->Weights(aWeights->ChangeArray1());
  }

  // Build flat knot sequence
  const int aDegree     = theCurve->Degree();
  const int aFlatLength = BSplCLib::KnotSequenceLength(theCurve->Multiplicities(),
                                                       aDegree,
                                                       theCurve->IsPeriodic());

  Handle(TColStd_HArray1OfReal) aFlatKnots = new TColStd_HArray1OfReal(1, aFlatLength);
  BSplCLib::KnotSequence(theCurve->Knots(),
                         theCurve->Multiplicities(),
                         aDegree,
                         theCurve->IsPeriodic(),
                         aFlatKnots->ChangeArray1());

  myIsInitialized = myEvaluator.Initialize(aDegree,
                                           aPoles,
                                           aWeights,
                                           aFlatKnots,
                                           theCurve->IsRational(),
                                           theCurve->IsPeriodic());
}
