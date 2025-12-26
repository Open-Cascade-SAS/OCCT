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

#include <ExtremaPS_OtherSurface.hxx>

//==================================================================================================

ExtremaPS_OtherSurface::ExtremaPS_OtherSurface(const Handle(Geom_Surface)& theSurface)
    : mySurface(theSurface),
      myAdaptor(theSurface)
{
}

//==================================================================================================

gp_Pnt ExtremaPS_OtherSurface::Value(double theU, double theV) const
{
  return myAdaptor.Value(theU, theV);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_OtherSurface::Perform(const gp_Pnt&              theP,
                                                   const ExtremaPS::Domain2D& theDomain,
                                                   double                     theTol,
                                                   ExtremaPS::SearchMode      theMode) const
{
  // Default grid density for general surfaces
  const int aNbUSamples = 32;
  const int aNbVSamples = 32;

  GeomGridEval_OtherSurface anEval(&myAdaptor);

  return ExtremaPS_GridEvaluator::PerformGridBased(anEval,
                                                    myAdaptor,
                                                    theP,
                                                    theDomain,
                                                    aNbUSamples,
                                                    aNbVSamples,
                                                    theTol,
                                                    theMode);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_OtherSurface::PerformWithBoundary(const gp_Pnt&              theP,
                                                               const ExtremaPS::Domain2D& theDomain,
                                                               double                     theTol,
                                                               ExtremaPS::SearchMode      theMode) const
{
  // Default grid density for general surfaces
  const int aNbUSamples = 32;
  const int aNbVSamples = 32;

  GeomGridEval_OtherSurface anEval(&myAdaptor);

  return ExtremaPS_GridEvaluator::PerformGridBasedWithBoundary(anEval,
                                                                myAdaptor,
                                                                theP,
                                                                theDomain,
                                                                aNbUSamples,
                                                                aNbVSamples,
                                                                theTol,
                                                                theMode);
}
