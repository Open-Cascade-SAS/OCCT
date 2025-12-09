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

#include <GeomGridEvaluator_Curve.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>

//==================================================================================================

void GeomGridEvaluator_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  myCurveType = theCurve.GetType();

  switch (myCurveType)
  {
    case GeomAbs_Line:
    {
      // Create Handle(Geom_Line) from gp_Lin
      Handle(Geom_Line) aGeomLine = new Geom_Line(theCurve.Line());
      myEvaluator                 = GeomGridEvaluator_Line(aGeomLine);
      break;
    }
    case GeomAbs_Circle:
    {
      // Create Handle(Geom_Circle) from gp_Circ
      Handle(Geom_Circle) aGeomCircle = new Geom_Circle(theCurve.Circle());
      myEvaluator                     = GeomGridEvaluator_Circle(aGeomCircle);
      break;
    }
    case GeomAbs_BSplineCurve:
    {
      myEvaluator = GeomGridEvaluator_BSplineCurve(theCurve.BSpline());
      break;
    }
    default:
    {
      // Fallback: use OtherCurve with adaptor copy
      myEvaluator = GeomGridEvaluator_OtherCurve(theCurve.ShallowCopy());
      break;
    }
  }
}

//==================================================================================================

void GeomGridEvaluator_Curve::SetParams(const TColStd_Array1OfReal& theParams)
{
  std::visit(
    [&theParams](auto& theEval)
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetParams(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool GeomGridEvaluator_Curve::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

int GeomGridEvaluator_Curve::NbParams() const
{
  return std::visit(
    [](const auto& theEval) -> int
    {
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

NCollection_Array1<gp_Pnt> GeomGridEvaluator_Curve::EvaluateGrid() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<gp_Pnt>
    {
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

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEvaluator_Curve::EvaluateGridD1() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD1>
    {
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

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEvaluator_Curve::EvaluateGridD2() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD2>
    {
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

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEvaluator_Curve::EvaluateGridD3() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array1<GeomGridEval::CurveD3>
    {
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
