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

#include <Geom2dGridEval_Curve.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>

namespace
{

//! Extracts basis curve from potentially nested TrimmedCurve wrappers.
//! @param theCurve input curve (may be TrimmedCurve or any other)
//! @return the underlying basis curve, or theCurve if not a TrimmedCurve
occ::handle<Geom2d_Curve> ExtractBasisCurve(const occ::handle<Geom2d_Curve>& theCurve)
{
  occ::handle<Geom2d_Curve> aResult = theCurve;
  while (auto aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(aResult))
  {
    aResult = aTrimmed->BasisCurve();
  }
  return aResult;
}

} // namespace

//==================================================================================================

void Geom2dGridEval_Curve::Initialize(const Adaptor2d_Curve2d& theCurve)
{
  if (theCurve.IsKind(STANDARD_TYPE(Geom2dAdaptor_Curve)))
  {
    Initialize(static_cast<const Geom2dAdaptor_Curve&>(theCurve).Curve());
    return;
  }

  // For non-Geom2dAdaptor or when Geom2d_Curve is not available,
  // use reference for the evaluator
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<Geom2dGridEval_OtherCurve>(theCurve);
}

//==================================================================================================

void Geom2dGridEval_Curve::Initialize(const occ::handle<Geom2d_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  // Extract basis curve from potentially nested TrimmedCurve wrappers
  occ::handle<Geom2d_Curve> aBasisCurve = ExtractBasisCurve(theCurve);

  if (auto aLine = occ::down_cast<Geom2d_Line>(aBasisCurve))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<Geom2dGridEval_Line>(aLine);
  }
  else if (auto aCircle = occ::down_cast<Geom2d_Circle>(aBasisCurve))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<Geom2dGridEval_Circle>(aCircle);
  }
  else if (auto anEllipse = occ::down_cast<Geom2d_Ellipse>(aBasisCurve))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<Geom2dGridEval_Ellipse>(anEllipse);
  }
  else if (auto aHyperbola = occ::down_cast<Geom2d_Hyperbola>(aBasisCurve))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<Geom2dGridEval_Hyperbola>(aHyperbola);
  }
  else if (auto aParabola = occ::down_cast<Geom2d_Parabola>(aBasisCurve))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<Geom2dGridEval_Parabola>(aParabola);
  }
  else if (auto aBezier = occ::down_cast<Geom2d_BezierCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<Geom2dGridEval_BezierCurve>(aBezier);
  }
  else if (auto aBSpline = occ::down_cast<Geom2d_BSplineCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<Geom2dGridEval_BSplineCurve>(aBSpline);
  }
  else if (auto anOffset = occ::down_cast<Geom2d_OffsetCurve>(aBasisCurve))
  {
    myCurveType = GeomAbs_OffsetCurve;
    myEvaluator.emplace<Geom2dGridEval_OffsetCurve>(anOffset);
  }
  else
  {
    // Unknown curve type - set uninitialized
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
  }
}

//==================================================================================================

bool Geom2dGridEval_Curve::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_Curve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  return std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<gp_Pnt2d> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Pnt2d>();
      }
      else
      {
        return theEval.EvaluateGrid(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_Curve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  return std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<Geom2dGridEval::CurveD1> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<Geom2dGridEval::CurveD1>();
      }
      else
      {
        return theEval.EvaluateGridD1(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_Curve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  return std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<Geom2dGridEval::CurveD2> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<Geom2dGridEval::CurveD2>();
      }
      else
      {
        return theEval.EvaluateGridD2(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_Curve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  return std::visit(
    [&theParams](const auto& theEval) -> NCollection_Array1<Geom2dGridEval::CurveD3> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<Geom2dGridEval::CurveD3>();
      }
      else
      {
        return theEval.EvaluateGridD3(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_Curve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  return std::visit(
    [&theParams, theN](const auto& theEval) -> NCollection_Array1<gp_Vec2d> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Vec2d>();
      }
      else
      {
        return theEval.EvaluateGridDN(theParams, theN);
      }
    },
    myEvaluator);
}
