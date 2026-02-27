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

#include <Geom2dProp_Curve.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const Adaptor2d_Curve2d& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const occ::handle<Geom2d_Curve>& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const Adaptor2d_Curve2d& theCurve)
{
  if (theCurve.IsKind(STANDARD_TYPE(Geom2dAdaptor_Curve)))
  {
    const auto& aGeomAdaptor = static_cast<const Geom2dAdaptor_Curve&>(theCurve);
    myAdaptor                = new Geom2dAdaptor_Curve(aGeomAdaptor);
    initFromAdaptor();
    return;
  }

  // For non-Geom2dAdaptor, set uninitialized.
  myAdaptor.Nullify();
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<std::monostate>();
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const occ::handle<Geom2d_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    myAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  myAdaptor = new Geom2dAdaptor_Curve(theCurve);
  initFromAdaptor();
}

//=================================================================================================

void Geom2dProp_Curve::initFromAdaptor()
{
  myCurveType                     = myAdaptor->GetType();
  const Geom2dAdaptor_Curve* aPtr = myAdaptor.get();

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator.emplace<Geom2dProp_Line>(aPtr);
      break;
    case GeomAbs_Circle:
      myEvaluator.emplace<Geom2dProp_Circle>(aPtr);
      break;
    case GeomAbs_Ellipse:
      myEvaluator.emplace<Geom2dProp_Ellipse>(aPtr);
      break;
    case GeomAbs_Hyperbola:
      myEvaluator.emplace<Geom2dProp_Hyperbola>(aPtr);
      break;
    case GeomAbs_Parabola:
      myEvaluator.emplace<Geom2dProp_Parabola>(aPtr);
      break;
    case GeomAbs_BezierCurve:
      myEvaluator.emplace<Geom2dProp_BezierCurve>(aPtr);
      break;
    case GeomAbs_BSplineCurve:
      myEvaluator.emplace<Geom2dProp_BSplineCurve>(aPtr);
      break;
    case GeomAbs_OffsetCurve:
      myEvaluator.emplace<Geom2dProp_OffsetCurve>(aPtr);
      break;
    default:
      myEvaluator.emplace<Geom2dProp_OtherCurve>(aPtr);
      break;
  }
}

//=================================================================================================

const Geom2dAdaptor_Curve* Geom2dProp_Curve::Adaptor() const
{
  return std::visit(
    [](const auto& theEval) -> const Geom2dAdaptor_Curve* {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
        return nullptr;
      else
        return theEval.Adaptor();
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::TangentResult Geom2dProp_Curve::Tangent(const double theParam,
                                                    const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::TangentResult {
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

//=================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_Curve::Curvature(const double theParam,
                                                        const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::CurvatureResult {
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

//=================================================================================================

Geom2dProp::NormalResult Geom2dProp_Curve::Normal(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::NormalResult {
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

//=================================================================================================

Geom2dProp::CentreResult Geom2dProp_Curve::CentreOfCurvature(const double theParam,
                                                             const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::CentreResult {
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

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_Curve::FindCurvatureExtrema() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveAnalysis {
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

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_Curve::FindInflections() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveAnalysis {
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
