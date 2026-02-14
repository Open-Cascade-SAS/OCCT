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

#include <ExtremaCC_CircleParabola.hxx>

#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>
#include <ExtremaCC_Parabola.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_CircleParabola::ExtremaCC_CircleParabola(const gp_Circ&  theCircle,
                                                   const gp_Parab& theParabola)
    : myCircle(theCircle),
      myParabola(theParabola),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_CircleParabola::ExtremaCC_CircleParabola(const gp_Circ&             theCircle,
                                                   const gp_Parab&            theParabola,
                                                   const ExtremaCC::Domain2D& theDomain)
    : myCircle(theCircle),
      myParabola(theParabola),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleParabola::Perform(double                theTol,
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
    aDomain.Curve2 = {-100.0, 100.0}; // Unbounded parabola - use reasonable range
  }

  ExtremaCC_Circle   aEval1(myCircle, aDomain.Curve1);
  ExtremaCC_Parabola aEval2(myParabola, aDomain.Curve2);

  ExtremaCC_GridEvaluator2D<ExtremaCC_Circle, ExtremaCC_Parabola> aGridEval(aEval1,
                                                                            aEval2,
                                                                            aDomain);
  aGridEval.Perform(myResult, theTol, theMode);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleParabola::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Circle   aEval1(myCircle, myDomain->Curve1);
    ExtremaCC_Parabola aEval2(myParabola, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}
