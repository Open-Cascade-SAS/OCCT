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

#include <ExtremaSS_GenericPair.hxx>

//==================================================================================================

ExtremaSS_GenericPair::ExtremaSS_GenericPair(const Adaptor3d_Surface& theSurface1,
                                             const Adaptor3d_Surface& theSurface2)
    : mySurface1(&theSurface1),
      mySurface2(&theSurface2),
      myDomain(std::nullopt),
      mySwapped(false),
      myNbSamplesU1(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesV1(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesU2(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesV2(ExtremaSS::THE_DEFAULT_NB_SAMPLES)
{
}

//==================================================================================================

ExtremaSS_GenericPair::ExtremaSS_GenericPair(const Adaptor3d_Surface&   theSurface1,
                                             const Adaptor3d_Surface&   theSurface2,
                                             const ExtremaSS::Domain4D& theDomain)
    : mySurface1(&theSurface1),
      mySurface2(&theSurface2),
      myDomain(theDomain),
      mySwapped(false),
      myNbSamplesU1(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesV1(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesU2(ExtremaSS::THE_DEFAULT_NB_SAMPLES),
      myNbSamplesV2(ExtremaSS::THE_DEFAULT_NB_SAMPLES)
{
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_GenericPair::Perform(double                theTol,
                                                        ExtremaSS::SearchMode theMode) const
{
  myResult.Clear();

  // TODO: Implement grid-based surface-surface extrema computation
  // 1. Build grids on both surfaces
  // 2. Scan grids to find candidate cells where surfaces are close
  // 3. Refine candidates using 4D Newton iteration
  // 4. Filter and store results

  (void)theTol;
  (void)theMode;

  myResult.Status = ExtremaSS::Status::NotDone;
  return myResult;
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_GenericPair::PerformWithBoundary(
  double                theTol,
  ExtremaSS::SearchMode theMode) const
{
  // Start with interior extrema
  (void)Perform(theTol, theMode);

  // TODO: Add boundary extrema handling

  return myResult;
}

//==================================================================================================

gp_Pnt ExtremaSS_GenericPair::Value1(double theU, double theV) const
{
  return mySurface1->Value(theU, theV);
}

//==================================================================================================

gp_Pnt ExtremaSS_GenericPair::Value2(double theU, double theV) const
{
  return mySurface2->Value(theU, theV);
}

//==================================================================================================

void ExtremaSS_GenericPair::buildGrids() const
{
  // TODO: Build evaluation grids for both surfaces
}

//==================================================================================================

void ExtremaSS_GenericPair::scanGrids(double theTol, ExtremaSS::SearchMode theMode) const
{
  (void)theTol;
  (void)theMode;
  // TODO: Scan grids to find candidate cells
}

//==================================================================================================

void ExtremaSS_GenericPair::refineCandidates(double theTol, ExtremaSS::SearchMode theMode) const
{
  (void)theTol;
  (void)theMode;
  // TODO: Refine candidates using 4D Newton
}
