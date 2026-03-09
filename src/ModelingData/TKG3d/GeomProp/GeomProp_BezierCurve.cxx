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

#include <GeomProp_BezierCurve.hxx>
#include <GeomProp_CurveAnalysisTools.pxx>

//=================================================================================================

GeomProp::TangentResult GeomProp_BezierCurve::Tangent(const double theParam,
                                                      const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return GeomProp_CurveAnalysisTools::EvaluateTangentCached(myCurve.get(),
                                                              theParam,
                                                              theTol,
                                                              myRequestedOrder,
                                                              myCache);
  }
  return GeomProp_CurveAnalysisTools::EvaluateTangentCached(myAdaptor,
                                                            theParam,
                                                            theTol,
                                                            myRequestedOrder,
                                                            myCache);
}

//=================================================================================================

GeomProp::CurvatureResult GeomProp_BezierCurve::Curvature(const double theParam,
                                                          const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return GeomProp_CurveAnalysisTools::EvaluateCurvatureCached(myCurve.get(),
                                                                theParam,
                                                                theTol,
                                                                myRequestedOrder,
                                                                myCache);
  }
  return GeomProp_CurveAnalysisTools::EvaluateCurvatureCached(myAdaptor,
                                                              theParam,
                                                              theTol,
                                                              myRequestedOrder,
                                                              myCache);
}

//=================================================================================================

GeomProp::NormalResult GeomProp_BezierCurve::Normal(const double theParam,
                                                    const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return GeomProp_CurveAnalysisTools::EvaluateNormalCached(myCurve.get(),
                                                             theParam,
                                                             theTol,
                                                             myRequestedOrder,
                                                             myCache);
  }
  return GeomProp_CurveAnalysisTools::EvaluateNormalCached(myAdaptor,
                                                           theParam,
                                                           theTol,
                                                           myRequestedOrder,
                                                           myCache);
}

//=================================================================================================

GeomProp::CentreResult GeomProp_BezierCurve::CentreOfCurvature(const double theParam,
                                                               const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return GeomProp_CurveAnalysisTools::EvaluateCentreOfCurvatureCached(myCurve.get(),
                                                                        theParam,
                                                                        theTol,
                                                                        myRequestedOrder,
                                                                        myCache);
  }
  return GeomProp_CurveAnalysisTools::EvaluateCentreOfCurvatureCached(myAdaptor,
                                                                      theParam,
                                                                      theTol,
                                                                      myRequestedOrder,
                                                                      myCache);
}

//=================================================================================================

GeomProp::CurveAnalysis GeomProp_BezierCurve::FindCurvatureExtrema() const
{
  if (!myCurve.IsNull())
  {
    // Create a stack-local adaptor for the span-based numeric search.
    const double      aFirst = myDomain.has_value() ? myDomain->First : myCurve->FirstParameter();
    const double      aLast  = myDomain.has_value() ? myDomain->Last : myCurve->LastParameter();
    GeomAdaptor_Curve anAdaptor(myCurve, aFirst, aLast);
    return GeomProp_CurveAnalysisTools::FindCurvatureExtrema(&anAdaptor);
  }
  return GeomProp_CurveAnalysisTools::FindCurvatureExtrema(myAdaptor);
}

//=================================================================================================

GeomProp::CurveAnalysis GeomProp_BezierCurve::FindInflections() const
{
  if (!myCurve.IsNull())
  {
    const double      aFirst = myDomain.has_value() ? myDomain->First : myCurve->FirstParameter();
    const double      aLast  = myDomain.has_value() ? myDomain->Last : myCurve->LastParameter();
    GeomAdaptor_Curve anAdaptor(myCurve, aFirst, aLast);
    return GeomProp_CurveAnalysisTools::FindInflections(&anAdaptor);
  }
  return GeomProp_CurveAnalysisTools::FindInflections(myAdaptor);
}
