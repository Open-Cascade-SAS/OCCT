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

#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>

//==================================================================================================

void GeomGridEvaluator_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  mySurfaceType = theSurface.GetType();

  switch (mySurfaceType)
  {
    case GeomAbs_Plane:
    {
      // Create Handle(Geom_Plane) from gp_Pln
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane(theSurface.Plane());
      myEvaluator                   = GeomGridEvaluator_Plane(aGeomPlane);
      break;
    }
    case GeomAbs_Sphere:
    {
      // Create Handle(Geom_SphericalSurface) from gp_Sphere
      Handle(Geom_SphericalSurface) aGeomSphere = new Geom_SphericalSurface(theSurface.Sphere());
      myEvaluator                               = GeomGridEvaluator_Sphere(aGeomSphere);
      break;
    }
    case GeomAbs_BSplineSurface:
    {
      myEvaluator = GeomGridEvaluator_BSplineSurface(theSurface.BSpline());
      break;
    }
    default:
    {
      // Fallback: use OtherSurface with adaptor copy
      myEvaluator = GeomGridEvaluator_OtherSurface(theSurface.ShallowCopy());
      break;
    }
  }
}

//==================================================================================================

void GeomGridEvaluator_Surface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                            const TColStd_Array1OfReal& theVParams)
{
  std::visit(
    [&theUParams, &theVParams](auto& theEval)
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetUVParams(theUParams, theVParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool GeomGridEvaluator_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
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

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEvaluator_Surface::EvaluateGridD1() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<GeomGridEval::SurfD1>
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array2<GeomGridEval::SurfD1>();
      }
      else
      {
        return theEval.EvaluateGridD1();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEvaluator_Surface::EvaluateGridD2() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<GeomGridEval::SurfD2>
    {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array2<GeomGridEval::SurfD2>();
      }
      else
      {
        return theEval.EvaluateGridD2();
      }
    },
    myEvaluator);
}
