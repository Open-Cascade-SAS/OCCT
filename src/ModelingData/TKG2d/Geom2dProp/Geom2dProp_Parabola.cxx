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

#include <Geom2dProp_Parabola.hxx>

//=================================================================================================

Geom2dProp::TangentResult Geom2dProp_Parabola::Tangent(const double theParam,
                                                       const double theTol) const
{
  if (myCurve.IsNull() && myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2, aD3;
  if (!myCurve.IsNull())
  {
    myCurve->D3(theParam, aPnt, aD1, aD2, aD3);
  }
  else
  {
    myAdaptor->D3(theParam, aPnt, aD1, aD2, aD3);
  }
  return Geom2dProp::ComputeTangent(aD1, aD2, aD3, theTol);
}

//=================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_Parabola::Curvature(const double theParam,
                                                           const double theTol) const
{
  if (myCurve.IsNull() && myAdaptor == nullptr)
  {
    return {0.0, false, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  if (!myCurve.IsNull())
  {
    myCurve->D2(theParam, aPnt, aD1, aD2);
  }
  else
  {
    myAdaptor->D2(theParam, aPnt, aD1, aD2);
  }
  return Geom2dProp::ComputeCurvature(aD1, aD2, theTol);
}

//=================================================================================================

Geom2dProp::NormalResult Geom2dProp_Parabola::Normal(const double theParam,
                                                     const double theTol) const
{
  if (myCurve.IsNull() && myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  if (!myCurve.IsNull())
  {
    myCurve->D2(theParam, aPnt, aD1, aD2);
  }
  else
  {
    myAdaptor->D2(theParam, aPnt, aD1, aD2);
  }
  return Geom2dProp::ComputeNormal(aD1, aD2, theTol);
}

//=================================================================================================

Geom2dProp::CentreResult Geom2dProp_Parabola::CentreOfCurvature(const double theParam,
                                                                const double theTol) const
{
  if (myCurve.IsNull() && myAdaptor == nullptr)
  {
    return {{}, false};
  }
  gp_Pnt2d aPnt;
  gp_Vec2d aD1, aD2;
  if (!myCurve.IsNull())
  {
    myCurve->D2(theParam, aPnt, aD1, aD2);
  }
  else
  {
    myAdaptor->D2(theParam, aPnt, aD1, aD2);
  }
  return Geom2dProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, theTol);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_Parabola::FindCurvatureExtrema() const
{
  Geom2dProp::CurveAnalysis aResult;
  aResult.IsDone = true;

  if (myCurve.IsNull() && myAdaptor == nullptr)
  {
    aResult.IsDone = false;
    return aResult;
  }

  double aUFirst, aULast;
  if (myDomain.has_value())
  {
    aUFirst = myDomain->First;
    aULast  = myDomain->Last;
  }
  else if (myAdaptor != nullptr)
  {
    aUFirst = myAdaptor->FirstParameter();
    aULast  = myAdaptor->LastParameter();
  }
  else
  {
    aUFirst = myCurve->FirstParameter();
    aULast  = myCurve->LastParameter();
  }

  // Parabola has maximum |curvature| at parameter 0 (vertex).
  if (aUFirst <= 0.0 && aULast >= 0.0)
  {
    aResult.Points.Append({0.0, Geom2dProp::CIType::MinCurvature});
  }

  return aResult;
}
