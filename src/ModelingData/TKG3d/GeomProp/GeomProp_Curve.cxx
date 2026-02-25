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

#include <GeomProp_Curve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>

//==================================================================================================

GeomProp_Curve::GeomProp_Curve(const Adaptor3d_Curve& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//==================================================================================================

GeomProp_Curve::GeomProp_Curve(const occ::handle<Geom_Curve>& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//==================================================================================================

void GeomProp_Curve::initialization(const Adaptor3d_Curve& theCurve)
{
  if (theCurve.IsKind(STANDARD_TYPE(GeomAdaptor_Curve)))
  {
    const auto& aGeomAdaptor = static_cast<const GeomAdaptor_Curve&>(theCurve);
    myAdaptor                = new GeomAdaptor_Curve(aGeomAdaptor);
    initFromAdaptor();
    return;
  }

  // For non-GeomAdaptor, set uninitialized.
  myAdaptor.Nullify();
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<std::monostate>();
}

//==================================================================================================

void GeomProp_Curve::initialization(const occ::handle<Geom_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    myAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  myAdaptor = new GeomAdaptor_Curve(theCurve);
  initFromAdaptor();
}

//==================================================================================================

void GeomProp_Curve::initFromAdaptor()
{
  myCurveType                   = myAdaptor->GetType();
  const GeomAdaptor_Curve* aPtr = myAdaptor.get();

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator.emplace<GeomProp_Line>(aPtr);
      break;
    case GeomAbs_Circle:
      myEvaluator.emplace<GeomProp_Circle>(aPtr);
      break;
    case GeomAbs_Ellipse:
      myEvaluator.emplace<GeomProp_Ellipse>(aPtr);
      break;
    case GeomAbs_Hyperbola:
      myEvaluator.emplace<GeomProp_Hyperbola>(aPtr);
      break;
    case GeomAbs_Parabola:
      myEvaluator.emplace<GeomProp_Parabola>(aPtr);
      break;
    case GeomAbs_BezierCurve:
      myEvaluator.emplace<GeomProp_BezierCurve>(aPtr);
      break;
    case GeomAbs_BSplineCurve:
      myEvaluator.emplace<GeomProp_BSplineCurve>(aPtr);
      break;
    case GeomAbs_OffsetCurve:
      myEvaluator.emplace<GeomProp_OffsetCurve>(aPtr);
      break;
    default:
      myEvaluator.emplace<GeomProp_OtherCurve>(aPtr);
      break;
  }
}

//==================================================================================================

GeomProp::TangentResult GeomProp_Curve::Tangent(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::TangentResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.Tangent(theParam, theTol);
      }
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::CurvatureResult GeomProp_Curve::Curvature(const double theParam,
                                                    const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::CurvatureResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {0.0, false, false};
      }
      else
      {
        return theEval.Curvature(theParam, theTol);
      }
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::NormalResult GeomProp_Curve::Normal(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::NormalResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.Normal(theParam, theTol);
      }
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::CentreResult GeomProp_Curve::CentreOfCurvature(const double theParam,
                                                         const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::CentreResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.CentreOfCurvature(theParam, theTol);
      }
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_Curve::FindCurvatureExtrema() const
{
  return std::visit(
    [](const auto& theEval) -> GeomProp::CurveAnalysis {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.FindCurvatureExtrema();
      }
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_Curve::FindInflections() const
{
  return std::visit(
    [](const auto& theEval) -> GeomProp::CurveAnalysis {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.FindInflections();
      }
    },
    myEvaluator);
}
