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

//==================================================================================================

void GeomGridEvaluator_Curve::Initialize(const Adaptor3d_Curve& theCurve)
{
  myCurveType = theCurve.GetType();

  switch (myCurveType)
  {
    case GeomAbs_Line:
    {
      GeomGridEvaluator_Line anEval;
      anEval.Initialize(theCurve.Line());
      myEvaluator = std::move(anEval);
      break;
    }
    case GeomAbs_Circle:
    {
      GeomGridEvaluator_Circle anEval;
      anEval.Initialize(theCurve.Circle());
      myEvaluator = std::move(anEval);
      break;
    }
    case GeomAbs_BSplineCurve:
    {
      GeomGridEvaluator_BSplineCurve anEval;
      anEval.Initialize(theCurve.BSpline());
      myEvaluator = std::move(anEval);
      break;
    }
    default:
    {
      // Fallback: use OtherCurve with adaptor copy
      GeomGridEvaluator_OtherCurve anEval;
      anEval.Initialize(theCurve.ShallowCopy());
      myEvaluator = std::move(anEval);
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
  return std::visit(
    [](const auto& theEval) -> bool
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return false;
      }
      else
      {
        return theEval.IsInitialized();
      }
    },
    myEvaluator);
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
