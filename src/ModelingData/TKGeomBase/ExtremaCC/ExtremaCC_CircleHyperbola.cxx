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

#include <ExtremaCC_CircleHyperbola.hxx>

#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <ExtremaCC_Hyperbola.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_CircleHyperbola::ExtremaCC_CircleHyperbola(const gp_Circ& theCircle,
                                                     const gp_Hypr& theHyperbola)
    : myCircle(theCircle),
      myHyperbola(theHyperbola),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_CircleHyperbola::ExtremaCC_CircleHyperbola(const gp_Circ&              theCircle,
                                                     const gp_Hypr&              theHyperbola,
                                                     const ExtremaCC::Domain2D& theDomain)
    : myCircle(theCircle),
      myHyperbola(theHyperbola),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleHyperbola::Perform(double                theTol,
                                                            ExtremaCC::SearchMode theMode) const
{
  myResult.Clear();

  ExtremaCC::Domain2D aDomain;
  if (myDomain.has_value())
  {
    aDomain = *myDomain;
  }
  else
  {
    aDomain.Curve1 = {0.0, 2.0 * M_PI};
    aDomain.Curve2 = {-10.0, 10.0}; // Unbounded hyperbola - use reasonable range
  }

  ExtremaCC_Circle    aEval1(myCircle, aDomain.Curve1);
  ExtremaCC_Hyperbola aEval2(myHyperbola, aDomain.Curve2);

  ExtremaCC_GridEvaluator2D<ExtremaCC_Circle, ExtremaCC_Hyperbola> aGridEval(aEval1,
                                                                             aEval2,
                                                                             aDomain);
  aGridEval.Perform(myResult, theTol, theMode);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleHyperbola::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Circle    aEval1(myCircle);
    ExtremaCC_Hyperbola aEval2(myHyperbola);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}
