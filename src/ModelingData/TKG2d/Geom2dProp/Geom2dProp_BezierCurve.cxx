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

#include <Geom2dProp_BezierCurve.hxx>
#include <Geom2dProp_CurveAnalysisTools.pxx>

//=================================================================================================

Geom2dProp::TangentResult Geom2dProp_BezierCurve::Tangent(const double theParam,
                                                          const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return Geom2dProp_CurveAnalysisTools::EvaluateTangentCached(myCurve.get(),
                                                                theParam,
                                                                theTol,
                                                                myRequestedOrder,
                                                                myCache);
  }
  return Geom2dProp_CurveAnalysisTools::EvaluateTangentCached(myAdaptor,
                                                              theParam,
                                                              theTol,
                                                              myRequestedOrder,
                                                              myCache);
}

//=================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_BezierCurve::Curvature(const double theParam,
                                                              const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return Geom2dProp_CurveAnalysisTools::EvaluateCurvatureCached(myCurve.get(),
                                                                  theParam,
                                                                  theTol,
                                                                  myRequestedOrder,
                                                                  myCache);
  }
  return Geom2dProp_CurveAnalysisTools::EvaluateCurvatureCached(myAdaptor,
                                                                theParam,
                                                                theTol,
                                                                myRequestedOrder,
                                                                myCache);
}

//=================================================================================================

Geom2dProp::NormalResult Geom2dProp_BezierCurve::Normal(const double theParam,
                                                        const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return Geom2dProp_CurveAnalysisTools::EvaluateNormalCached(myCurve.get(),
                                                               theParam,
                                                               theTol,
                                                               myRequestedOrder,
                                                               myCache);
  }
  return Geom2dProp_CurveAnalysisTools::EvaluateNormalCached(myAdaptor,
                                                             theParam,
                                                             theTol,
                                                             myRequestedOrder,
                                                             myCache);
}

//=================================================================================================

Geom2dProp::CentreResult Geom2dProp_BezierCurve::CentreOfCurvature(const double theParam,
                                                                   const double theTol) const
{
  if (!myCurve.IsNull())
  {
    return Geom2dProp_CurveAnalysisTools::EvaluateCentreOfCurvatureCached(myCurve.get(),
                                                                          theParam,
                                                                          theTol,
                                                                          myRequestedOrder,
                                                                          myCache);
  }
  return Geom2dProp_CurveAnalysisTools::EvaluateCentreOfCurvatureCached(myAdaptor,
                                                                        theParam,
                                                                        theTol,
                                                                        myRequestedOrder,
                                                                        myCache);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_BezierCurve::FindCurvatureExtrema() const
{
  if (!myCurve.IsNull())
  {
    // Create a stack-local adaptor for the span-based numeric search.
    const double          aFirst = myDomain.has_value() ? myDomain->First : myCurve->FirstParameter();
    const double          aLast  = myDomain.has_value() ? myDomain->Last : myCurve->LastParameter();
    Geom2dAdaptor_Curve   anAdaptor(myCurve, aFirst, aLast);
    return Geom2dProp_CurveAnalysisTools::FindCurvatureExtrema(&anAdaptor);
  }
  return Geom2dProp_CurveAnalysisTools::FindCurvatureExtrema(myAdaptor);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_BezierCurve::FindInflections() const
{
  if (!myCurve.IsNull())
  {
    const double          aFirst = myDomain.has_value() ? myDomain->First : myCurve->FirstParameter();
    const double          aLast  = myDomain.has_value() ? myDomain->Last : myCurve->LastParameter();
    Geom2dAdaptor_Curve   anAdaptor(myCurve, aFirst, aLast);
    return Geom2dProp_CurveAnalysisTools::FindInflections(&anAdaptor);
  }
  return Geom2dProp_CurveAnalysisTools::FindInflections(myAdaptor);
}
