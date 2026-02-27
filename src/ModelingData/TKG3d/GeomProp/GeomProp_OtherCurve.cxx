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

#include <GeomProp_OtherCurve.hxx>
#include <GeomProp_CurveAnalysisTools.pxx>

//=================================================================================================

GeomProp::TangentResult GeomProp_OtherCurve::Tangent(const double theParam,
                                                     const double theTol) const
{
  return GeomProp_CurveAnalysisTools::EvaluateTangent(myAdaptor, theParam, theTol);
}

//=================================================================================================

GeomProp::CurvatureResult GeomProp_OtherCurve::Curvature(const double theParam,
                                                         const double theTol) const
{
  return GeomProp_CurveAnalysisTools::EvaluateCurvature(myAdaptor, theParam, theTol);
}

//=================================================================================================

GeomProp::NormalResult GeomProp_OtherCurve::Normal(const double theParam, const double theTol) const
{
  return GeomProp_CurveAnalysisTools::EvaluateNormal(myAdaptor, theParam, theTol);
}

//=================================================================================================

GeomProp::CentreResult GeomProp_OtherCurve::CentreOfCurvature(const double theParam,
                                                              const double theTol) const
{
  return GeomProp_CurveAnalysisTools::EvaluateCentreOfCurvature(myAdaptor, theParam, theTol);
}

//=================================================================================================

GeomProp::CurveAnalysis GeomProp_OtherCurve::FindCurvatureExtrema() const
{
  return GeomProp_CurveAnalysisTools::FindCurvatureExtrema(myAdaptor);
}

//=================================================================================================

GeomProp::CurveAnalysis GeomProp_OtherCurve::FindInflections() const
{
  return GeomProp_CurveAnalysisTools::FindInflections(myAdaptor);
}
