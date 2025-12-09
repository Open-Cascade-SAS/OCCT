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

#include <GeomGridEvaluator_Surface.hxx>

//==================================================================================================

void GeomGridEvaluator_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  mySurfaceType = theSurface.GetType();

  switch (mySurfaceType)
  {
    case GeomAbs_Plane:
    {
      GeomGridEvaluator_Plane anEval;
      anEval.Initialize(theSurface.Plane());
      myEvaluator = std::move(anEval);
      break;
    }
    case GeomAbs_Sphere:
    {
      GeomGridEvaluator_Sphere anEval;
      anEval.Initialize(theSurface.Sphere());
      myEvaluator = std::move(anEval);
      break;
    }
    case GeomAbs_BSplineSurface:
    {
      GeomGridEvaluator_BSplineSurface anEval;
      anEval.Initialize(theSurface.BSpline());
      myEvaluator = std::move(anEval);
      break;
    }
    default:
    {
      // Fallback: use OtherSurface with adaptor copy
      GeomGridEvaluator_OtherSurface anEval;
      anEval.Initialize(theSurface.ShallowCopy());
      myEvaluator = std::move(anEval);
      break;
    }
  }
}

//==================================================================================================

void GeomGridEvaluator_Surface::SetUParams(const TColStd_Array1OfReal& theParams)
{
  std::visit(
    [&theParams](auto& theEval)
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetUParams(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

void GeomGridEvaluator_Surface::SetVParams(const TColStd_Array1OfReal& theParams)
{
  std::visit(
    [&theParams](auto& theEval)
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetVParams(theParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool GeomGridEvaluator_Surface::IsInitialized() const
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

int GeomGridEvaluator_Surface::NbUParams() const
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
        return theEval.NbUParams();
      }
    },
    myEvaluator);
}

//==================================================================================================

int GeomGridEvaluator_Surface::NbVParams() const
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
        return theEval.NbVParams();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEvaluator_Surface::EvaluateGrid() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<gp_Pnt>
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array2<gp_Pnt>();
      }
      else
      {
        return theEval.EvaluateGrid();
      }
    },
    myEvaluator);
}
