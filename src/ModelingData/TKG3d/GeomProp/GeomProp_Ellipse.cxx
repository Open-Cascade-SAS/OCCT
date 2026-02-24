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

#include <GeomProp_Ellipse.hxx>

#include <ElCLib.hxx>

#include <cmath>

namespace
{
constexpr int    THE_ELLIPSE_NB_EXTREMA = 4; //!< Number of curvature extrema on full ellipse
constexpr double THE_ELLIPSE_PERIOD     = 2.0 * M_PI; //!< One full period of ellipse parameter
} // namespace

//==================================================================================================

GeomProp::TangentResult GeomProp_Ellipse::Tangent(const double theParam, const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2, aD3;
  myAdaptor->D3(theParam, aPnt, aD1, aD2, aD3);
  return GeomProp::ComputeTangent(aD1, aD2, aD3, theTol);
}

//==================================================================================================

GeomProp::CurvatureResult GeomProp_Ellipse::Curvature(const double theParam,
                                                       const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {0.0, false, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCurvature(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::NormalResult GeomProp_Ellipse::Normal(const double theParam, const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeNormal(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::CentreResult GeomProp_Ellipse::CentreOfCurvature(const double theParam,
                                                            const double theTol) const
{
  if (myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myAdaptor->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_Ellipse::FindCurvatureExtrema() const
{
  GeomProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  const double aUFirst    = myAdaptor->FirstParameter();
  const double aULast     = myAdaptor->LastParameter();
  const double aUFPlus2PI = aUFirst + THE_ELLIPSE_PERIOD;

  // Ellipse curvature extrema at 0, PI/2, PI, 3*PI/2
  // At 0 and PI (major axis endpoints): min radius of curvature -> max |curvature| -> MinCurvature
  // At PI/2 and 3*PI/2 (minor axis endpoints): max radius of curvature -> min |curvature| ->
  // MaxCurvature
  const double aCandidates[] = {0.0, M_PI / 2.0, M_PI, 3.0 * M_PI / 2.0};
  const bool   aIsMin[]      = {true, false, true, false};

  for (int i = 0; i < THE_ELLIPSE_NB_EXTREMA; ++i)
  {
    const double aU = ElCLib::InPeriod(aCandidates[i], aUFirst, aUFPlus2PI);
    if (aU >= aUFirst && aU <= aULast)
    {
      const GeomProp::CIType aType =
        aIsMin[i] ? GeomProp::CIType::MinCurvature : GeomProp::CIType::MaxCurvature;
      aResult.Points.Append({aU, aType});
    }
  }

  return aResult;
}
