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

#include <ExtremaPC_Curve.hxx>

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve()
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  // Default values are set in Config struct initialization
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  Initialize(theCurve);
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  Initialize(theCurve, theUMin, theUMax);
}

//==================================================================================================

void ExtremaPC_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  Initialize(theCurve, theCurve.FirstParameter(), theCurve.LastParameter());
}

//==================================================================================================

void ExtremaPC_Curve::Initialize(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax)
{
  myCurveType    = theCurve.GetType();
  myConfig.UMin  = theUMin;
  myConfig.UMax  = theUMax;

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line());
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle());
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse());
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola());
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola());
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier());
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline());
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve);
      break;
  }
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_Curve::Perform(const gp_Pnt& theP) const
{
  ExtremaPC::Domain1D aDomain(myConfig.UMin, myConfig.UMax);

  return std::visit(
    [&](const auto& theEval) -> ExtremaPC::Result {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        ExtremaPC::Result aResult;
        aResult.Status = ExtremaPC::Status::NotDone;
        return aResult;
      }
      else
      {
        if (myConfig.IncludeEndpoints)
        {
          return theEval.PerformWithEndpoints(theP, aDomain, myConfig.Tolerance, myConfig.Mode);
        }
        else
        {
          return theEval.Perform(theP, aDomain, myConfig.Tolerance, myConfig.Mode);
        }
      }
    },
    myEvaluator);
}
