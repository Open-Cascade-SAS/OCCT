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

#include <Geom2dProp_BSplineCurve.hxx>
#include <Geom2dProp_CurveAnalysisTools.pxx>

#include <GeomAbs_Shape.hxx>

//=================================================================================================

Geom2dProp::TangentResult Geom2dProp_BSplineCurve::Tangent(const double theParam,
                                                           const double theTol) const
{
  return Geom2dProp_CurveAnalysisTools::EvaluateTangentCached(myAdaptor,
                                                              theParam,
                                                              theTol,
                                                              myRequestedOrder,
                                                              myCache);
}

//=================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_BSplineCurve::Curvature(const double theParam,
                                                               const double theTol) const
{
  return Geom2dProp_CurveAnalysisTools::EvaluateCurvatureCached(myAdaptor,
                                                                theParam,
                                                                theTol,
                                                                myRequestedOrder,
                                                                myCache);
}

//=================================================================================================

Geom2dProp::NormalResult Geom2dProp_BSplineCurve::Normal(const double theParam,
                                                         const double theTol) const
{
  return Geom2dProp_CurveAnalysisTools::EvaluateNormalCached(myAdaptor,
                                                             theParam,
                                                             theTol,
                                                             myRequestedOrder,
                                                             myCache);
}

//=================================================================================================

Geom2dProp::CentreResult Geom2dProp_BSplineCurve::CentreOfCurvature(const double theParam,
                                                                    const double theTol) const
{
  return Geom2dProp_CurveAnalysisTools::EvaluateCentreOfCurvatureCached(myAdaptor,
                                                                        theParam,
                                                                        theTol,
                                                                        myRequestedOrder,
                                                                        myCache);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_BSplineCurve::FindCurvatureExtrema() const
{
  return Geom2dProp_CurveAnalysisTools::FindCurvatureExtremaBySpans(myAdaptor, GeomAbs_C3);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_BSplineCurve::FindInflections() const
{
  return Geom2dProp_CurveAnalysisTools::FindInflectionsBySpans(myAdaptor, GeomAbs_C3);
}
