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

#include <ExtremaCC_EllipseEllipse.hxx>

#include <ExtremaCC_Ellipse.hxx>
#include <ExtremaCC_GridEvaluator2D.hxx>

#include <cmath>

//==================================================================================================

ExtremaCC_EllipseEllipse::ExtremaCC_EllipseEllipse(const gp_Elips& theEllipse1,
                                                   const gp_Elips& theEllipse2)
    : myEllipse1(theEllipse1),
      myEllipse2(theEllipse2),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_EllipseEllipse::ExtremaCC_EllipseEllipse(const gp_Elips&             theEllipse1,
                                                   const gp_Elips&             theEllipse2,
                                                   const ExtremaCC::Domain2D& theDomain)
    : myEllipse1(theEllipse1),
      myEllipse2(theEllipse2),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseEllipse::Perform(double                theTol,
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
    aDomain.Curve2 = {0.0, 2.0 * M_PI};
  }

  ExtremaCC_Ellipse aEval1(myEllipse1, aDomain.Curve1);
  ExtremaCC_Ellipse aEval2(myEllipse2, aDomain.Curve2);

  ExtremaCC_GridEvaluator2D<ExtremaCC_Ellipse, ExtremaCC_Ellipse> aGridEval(aEval1,
                                                                            aEval2,
                                                                            aDomain);
  aGridEval.Perform(myResult, theTol, theMode);

  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_EllipseEllipse::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Ellipse aEval1(myEllipse1, myDomain->Curve1);
    ExtremaCC_Ellipse aEval2(myEllipse2, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}
