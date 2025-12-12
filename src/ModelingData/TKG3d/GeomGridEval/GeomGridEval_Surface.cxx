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

#include <GeomGridEval_Surface.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

//==================================================================================================

void GeomGridEval_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  // Try to downcast to GeomAdaptor_Surface to get underlying Geom_Surface
  const GeomAdaptor_Surface* aGeomAdaptor = dynamic_cast<const GeomAdaptor_Surface*>(&theSurface);
  if (aGeomAdaptor != nullptr)
  {
    const Handle(Geom_Surface)& aGeomSurf = aGeomAdaptor->Surface();
    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }
  }

  // For non-GeomAdaptor or when Geom_Surface is not available,
  // use reference for the evaluator
  mySurfaceType = theSurface.GetType();

  switch (mySurfaceType)
  {
    case GeomAbs_Plane: {
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane(theSurface.Plane());
      myEvaluator.emplace<GeomGridEval_Plane>(aGeomPlane);
      break;
    }
    case GeomAbs_Cylinder: {
      Handle(Geom_CylindricalSurface) aGeomCyl = new Geom_CylindricalSurface(theSurface.Cylinder());
      myEvaluator.emplace<GeomGridEval_Cylinder>(aGeomCyl);
      break;
    }
    case GeomAbs_Sphere: {
      Handle(Geom_SphericalSurface) aGeomSphere = new Geom_SphericalSurface(theSurface.Sphere());
      myEvaluator.emplace<GeomGridEval_Sphere>(aGeomSphere);
      break;
    }
    case GeomAbs_Cone: {
      Handle(Geom_ConicalSurface) aGeomCone = new Geom_ConicalSurface(theSurface.Cone());
      myEvaluator.emplace<GeomGridEval_Cone>(aGeomCone);
      break;
    }
    case GeomAbs_Torus: {
      Handle(Geom_ToroidalSurface) aGeomTorus = new Geom_ToroidalSurface(theSurface.Torus());
      myEvaluator.emplace<GeomGridEval_Torus>(aGeomTorus);
      break;
    }
    case GeomAbs_BezierSurface: {
      myEvaluator.emplace<GeomGridEval_BezierSurface>(theSurface.Bezier());
      break;
    }
    case GeomAbs_BSplineSurface: {
      myEvaluator.emplace<GeomGridEval_BSplineSurface>(theSurface.BSpline());
      break;
    }
    default: {
      // Fallback: store reference for OtherSurface
      myEvaluator.emplace<GeomGridEval_OtherSurface>(theSurface);
      break;
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::Initialize(const Handle(Geom_Surface)& theSurface)
{
  if (theSurface.IsNull())
  {
    myEvaluator.emplace<std::monostate>();
    mySurfaceType = GeomAbs_OtherSurface;
    return;
  }

  if (auto aPlane = Handle(Geom_Plane)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Plane;
    myEvaluator.emplace<GeomGridEval_Plane>(aPlane);
  }
  else if (auto aCyl = Handle(Geom_CylindricalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Cylinder;
    myEvaluator.emplace<GeomGridEval_Cylinder>(aCyl);
  }
  else if (auto aSphere = Handle(Geom_SphericalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Sphere;
    myEvaluator.emplace<GeomGridEval_Sphere>(aSphere);
  }
  else if (auto aCone = Handle(Geom_ConicalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Cone;
    myEvaluator.emplace<GeomGridEval_Cone>(aCone);
  }
  else if (auto aTorus = Handle(Geom_ToroidalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Torus;
    myEvaluator.emplace<GeomGridEval_Torus>(aTorus);
  }
  else if (auto aBezier = Handle(Geom_BezierSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_BezierSurface;
    myEvaluator.emplace<GeomGridEval_BezierSurface>(aBezier);
  }
  else if (auto aBSpline = Handle(Geom_BSplineSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_BSplineSurface;
    myEvaluator.emplace<GeomGridEval_BSplineSurface>(aBSpline);
  }
  else if (auto anOffset = Handle(Geom_OffsetSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_OffsetSurface;
    myEvaluator.emplace<GeomGridEval_OffsetSurface>(anOffset);
  }
  else
  {
    // Unknown surface type - set uninitialized
    // All known Geom_Surface types are handled above
    myEvaluator.emplace<std::monostate>();
    mySurfaceType = GeomAbs_OtherSurface;
  }
}

//==================================================================================================

void GeomGridEval_Surface::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                       const TColStd_Array1OfReal& theVParams)
{
  std::visit(
    [&theUParams, &theVParams](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetUVParams(theUParams, theVParams);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool GeomGridEval_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

int GeomGridEval_Surface::NbUParams() const
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
        return theEval.NbUParams();
      }
    },
    myEvaluator);
}

//==================================================================================================

int GeomGridEval_Surface::NbVParams() const
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
        return theEval.NbVParams();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_Surface::EvaluateGrid() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<gp_Pnt> {
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

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Surface::EvaluateGridD1() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<GeomGridEval::SurfD1> {
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

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Surface::EvaluateGridD2() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<GeomGridEval::SurfD2> {
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

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Surface::EvaluateGridD3() const
{
  return std::visit(
    [](const auto& theEval) -> NCollection_Array2<GeomGridEval::SurfD3> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array2<GeomGridEval::SurfD3>();
      }
      else
      {
        return theEval.EvaluateGridD3();
      }
    },
    myEvaluator);
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_Surface::EvaluateGridDN(int theNU, int theNV) const
{
  return std::visit(
    [theNU, theNV](const auto& theEval) -> NCollection_Array2<gp_Vec> {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return NCollection_Array2<gp_Vec>();
      }
      else
      {
        return theEval.EvaluateGridDN(theNU, theNV);
      }
    },
    myEvaluator);
}
