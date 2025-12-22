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

#include <GeomGridEval_Curve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>

namespace
{

//! Extracts basis curve from potentially nested TrimmedCurve wrappers.
//! @param theCurve input curve (may be TrimmedCurve or any other)
//! @return the underlying basis curve, or theCurve if not a TrimmedCurve
Handle(Geom_Curve) ExtractBasisCurve(const Handle(Geom_Curve)& theCurve)
{
  Handle(Geom_Curve) aResult = theCurve;
  while (auto aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(aResult))
  {
    aResult = aTrimmed->BasisCurve();
  }
  return aResult;
}

} // namespace

//==================================================================================================

void GeomGridEval_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  if (theCurve.IsInstance(STANDARD_TYPE(GeomAdaptor_Curve)))
  {
    Initialize(static_cast<const GeomAdaptor_Curve&>(theCurve).Curve());
    return;
  }

  // For non-GeomAdaptor or when Geom_Curve is not available,
  // use reference for the evaluator
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<GeomGridEval_OtherCurve>(theCurve);
}

//==================================================================================================

void GeomGridEval_Curve::Initialize(const Handle(Geom_Curve)& theCurve)
{
  if (theCurve.IsNull())
  {
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  // Extract basis curve from potentially nested TrimmedCurve wrappers
  Handle(Geom_Curve) aBasisCurve = ExtractBasisCurve(theCurve);

  if (auto aLine = Handle(Geom_Line)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<GeomGridEval_Line>(aLine);
  }
  else if (auto aCircle = Handle(Geom_Circle)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<GeomGridEval_Circle>(aCircle);
  }
  else if (auto anEllipse = Handle(Geom_Ellipse)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<GeomGridEval_Ellipse>(anEllipse);
  }
  else if (auto aHyperbola = Handle(Geom_Hyperbola)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<GeomGridEval_Hyperbola>(aHyperbola);
  }
  else if (auto aParabola = Handle(Geom_Parabola)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<GeomGridEval_Parabola>(aParabola);
  }
  else if (auto aBezier = Handle(Geom_BezierCurve)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<GeomGridEval_BezierCurve>(aBezier);
  }
  else if (auto aBSpline = Handle(Geom_BSplineCurve)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<GeomGridEval_BSplineCurve>(aBSpline);
  }
  else if (auto anOffset = Handle(Geom_OffsetCurve)::DownCast(aBasisCurve))
  {
    myCurveType = GeomAbs_OffsetCurve;
    myEvaluator.emplace<GeomGridEval_OffsetCurve>(anOffset);
  }
  else
  {
    // Unknown curve type - set uninitialized
    // All known Geom_Curve types are handled above
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
  }
}

//==================================================================================================

void GeomGridEval_Curve::SetParams(const TColStd_Array1OfReal& theParams)
{
  std::visit(
    [&theParams](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetParams(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool GeomGridEval_Curve::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

int GeomGridEval_Curve::NbParams() const
{
  return std::visit(
    [](const auto& theEval) -> int {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return 0;
      }
      else
      {
        return theEval.NbParams();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_Curve::EvaluateGrid() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<gp_Pnt> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Pnt>();
      }
      else
      {
        return theEval.EvaluateGrid();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_Curve::EvaluateGridD1() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD1> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD1>();
      }
      else
      {
        return theEval.EvaluateGridD1();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_Curve::EvaluateGridD2() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD2> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD2>();
      }
      else
      {
        return theEval.EvaluateGridD2();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_Curve::EvaluateGridD3() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD3> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<GeomGridEval::CurveD3>();
      }
      else
      {
        return theEval.EvaluateGridD3();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_Curve::EvaluateGridDN(int theN) const
{
  return std::visit(
    [theN](const auto& theEval) -> NCollection_Array1<gp_Vec> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array1<gp_Vec>();
      }
      else
      {
        return theEval.EvaluateGridDN(theN);
      }
    },
    myEvaluator);
}
