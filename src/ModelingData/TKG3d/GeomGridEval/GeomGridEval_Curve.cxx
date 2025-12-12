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

//==================================================================================================

void GeomGridEval_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  // Try to downcast to GeomAdaptor_Curve to get underlying Geom_Curve
  const GeomAdaptor_Curve* aGeomAdaptor = dynamic_cast<const GeomAdaptor_Curve*>(&theCurve);
  if (aGeomAdaptor != nullptr)
  {
    const Handle(Geom_Curve)& aGeomCurve = aGeomAdaptor->Curve();
    if (!aGeomCurve.IsNull())
    {
      Initialize(aGeomCurve);
      return;
    }
  }

  // For non-GeomAdaptor or when Geom_Curve is not available,
  // use reference for the evaluator
  myCurveType = theCurve.GetType();

  switch (myCurveType)
  {
    case GeomAbs_Line: {
      Handle(Geom_Line) aGeomLine = new Geom_Line(theCurve.Line());
      myEvaluator.emplace<GeomGridEval_Line>(aGeomLine);
      break;
    }
    case GeomAbs_Circle: {
      Handle(Geom_Circle) aGeomCircle = new Geom_Circle(theCurve.Circle());
      myEvaluator.emplace<GeomGridEval_Circle>(aGeomCircle);
      break;
    }
    case GeomAbs_Ellipse: {
      Handle(Geom_Ellipse) aGeomEllipse = new Geom_Ellipse(theCurve.Ellipse());
      myEvaluator.emplace<GeomGridEval_Ellipse>(aGeomEllipse);
      break;
    }
    case GeomAbs_Hyperbola: {
      Handle(Geom_Hyperbola) aGeomHyperbola = new Geom_Hyperbola(theCurve.Hyperbola());
      myEvaluator.emplace<GeomGridEval_Hyperbola>(aGeomHyperbola);
      break;
    }
    case GeomAbs_Parabola: {
      Handle(Geom_Parabola) aGeomParabola = new Geom_Parabola(theCurve.Parabola());
      myEvaluator.emplace<GeomGridEval_Parabola>(aGeomParabola);
      break;
    }
    case GeomAbs_BezierCurve: {
      myEvaluator.emplace<GeomGridEval_BezierCurve>(theCurve.Bezier());
      break;
    }
    case GeomAbs_BSplineCurve: {
      myEvaluator.emplace<GeomGridEval_BSplineCurve>(theCurve.BSpline());
      break;
    }
    case GeomAbs_OffsetCurve: {
      myEvaluator.emplace<GeomGridEval_OffsetCurve>(theCurve.OffsetCurve());
      break;
    }
    default: {
      // Fallback: store reference for OtherCurve
      myEvaluator.emplace<GeomGridEval_OtherCurve>(theCurve);
      break;
    }
  }
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

  if (auto aLine = Handle(Geom_Line)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<GeomGridEval_Line>(aLine);
  }
  else if (auto aCircle = Handle(Geom_Circle)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<GeomGridEval_Circle>(aCircle);
  }
  else if (auto anEllipse = Handle(Geom_Ellipse)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<GeomGridEval_Ellipse>(anEllipse);
  }
  else if (auto aHyperbola = Handle(Geom_Hyperbola)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<GeomGridEval_Hyperbola>(aHyperbola);
  }
  else if (auto aParabola = Handle(Geom_Parabola)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<GeomGridEval_Parabola>(aParabola);
  }
  else if (auto aBezier = Handle(Geom_BezierCurve)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<GeomGridEval_BezierCurve>(aBezier);
  }
  else if (auto aBSpline = Handle(Geom_BSplineCurve)::DownCast(theCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<GeomGridEval_BSplineCurve>(aBSpline);
  }
  else if (auto anOffset = Handle(Geom_OffsetCurve)::DownCast(theCurve))
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
