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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const Adaptor2d_Curve2d&            theCurve,
                                   const Geom2dProp::CurveDerivOrder   theOrder)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve, theOrder);
}

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const occ::handle<Geom2d_Curve>& theCurve,
                                   const Geom2dProp::CurveDerivOrder theOrder)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve, theOrder);
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const Adaptor2d_Curve2d&          theCurve,
                                      const Geom2dProp::CurveDerivOrder theOrder)
{
  if (theCurve.IsKind(STANDARD_TYPE(Geom2dAdaptor_Curve)))
  {
    const Geom2dAdaptor_Curve& aGeomAdaptor = static_cast<const Geom2dAdaptor_Curve&>(theCurve);
    if (!aGeomAdaptor.Curve().IsNull())
    {
      // Dispatch specialized evaluators directly from the GeomAdaptor.
      myOwnedAdaptor.Nullify();
      myCurveType = aGeomAdaptor.GetType();
      switch (myCurveType)
      {
        case GeomAbs_Line:
          myEvaluator.emplace<Geom2dProp_Line>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Circle:
          myEvaluator.emplace<Geom2dProp_Circle>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Ellipse:
          myEvaluator.emplace<Geom2dProp_Ellipse>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Hyperbola:
          myEvaluator.emplace<Geom2dProp_Hyperbola>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Parabola:
          myEvaluator.emplace<Geom2dProp_Parabola>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_BezierCurve:
          myEvaluator.emplace<Geom2dProp_BezierCurve>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_BSplineCurve:
          myEvaluator.emplace<Geom2dProp_BSplineCurve>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_OffsetCurve:
          myEvaluator.emplace<Geom2dProp_OffsetCurve>(&aGeomAdaptor, theOrder);
          break;
        default:
          myEvaluator.emplace<Geom2dProp_OtherCurve>(&theCurve, theOrder);
          break;
      }
      return;
    }
  }

  // Non-Geom adaptor or empty curve handle: use OtherCurve with adaptor pointer.
  myOwnedAdaptor.Nullify();
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<Geom2dProp_OtherCurve>(&theCurve, theOrder);
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const occ::handle<Geom2d_Curve>&  theCurve,
                                      const Geom2dProp::CurveDerivOrder theOrder)
{
  if (theCurve.IsNull())
  {
    myOwnedAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  // No adaptor creation for the handle path.
  myOwnedAdaptor.Nullify();

  // Unwrap TrimmedCurve to basis curve + optional CurveDomain.
  occ::handle<Geom2d_Curve>              aBasis = theCurve;
  std::optional<Geom2dProp::CurveDomain> aDomain;
  if (aBasis->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    aDomain = Geom2dProp::CurveDomain{theCurve->FirstParameter(), theCurve->LastParameter()};
    while (aBasis->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      aBasis = occ::down_cast<Geom2d_TrimmedCurve>(aBasis)->BasisCurve();
    }
  }

  // Type detection using DynamicType() - extracted once to avoid repeated virtual calls.
  const occ::handle<Standard_Type>& aType = aBasis->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<Geom2dProp_Line>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<Geom2dProp_Circle>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<Geom2dProp_Ellipse>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<Geom2dProp_Hyperbola>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<Geom2dProp_Parabola>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<Geom2dProp_BezierCurve>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<Geom2dProp_BSplineCurve>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    myCurveType = GeomAbs_OffsetCurve;
    myEvaluator.emplace<Geom2dProp_OffsetCurve>(aBasis, aDomain, theOrder);
  }
  else
  {
    myCurveType = GeomAbs_OtherCurve;
    myEvaluator.emplace<Geom2dProp_OtherCurve>(aBasis, aDomain, theOrder);
  }
}

//=================================================================================================

void Geom2dProp_Curve::SetDerivOrder(const Geom2dProp::CurveDerivOrder theOrder)
{
  std::visit(
    [theOrder](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetDerivOrder(theOrder);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CurveDerivOrder Geom2dProp_Curve::DerivOrder() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveDerivOrder {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Geom2dProp::CurveDerivOrder::Undefined;
      }
      else
      {
        return theEval.DerivOrder();
      }
    },
    myEvaluator);
}

//=================================================================================================

const Adaptor2d_Curve2d* Geom2dProp_Curve::Adaptor() const
{
  if (!myOwnedAdaptor.IsNull())
  {
    return myOwnedAdaptor.get();
  }
  return nullptr;
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
