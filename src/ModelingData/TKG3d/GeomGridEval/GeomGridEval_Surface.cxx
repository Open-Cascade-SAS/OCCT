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

void GeomGridEval_Surface::Initialize(const Handle(Adaptor3d_Surface)& theSurface)
{
  if (theSurface.IsNull())
  {
    myEvaluator = std::monostate{};
    mySurfaceType = GeomAbs_OtherSurface;
    return;
  }

  // Check if it is a GeomAdaptor_Surface
  Handle(GeomAdaptor_Surface) aGeomAdaptor = Handle(GeomAdaptor_Surface)::DownCast(theSurface);
  if (!aGeomAdaptor.IsNull())
  {
    Initialize(aGeomAdaptor->Surface());
    return;
  }

  mySurfaceType = theSurface->GetType();

  switch (mySurfaceType)
  {
    case GeomAbs_Plane:
    {
      // Create Handle(Geom_Plane) from gp_Pln
      Handle(Geom_Plane) aGeomPlane = new Geom_Plane(theSurface->Plane());
      myEvaluator                   = GeomGridEval_Plane(aGeomPlane);
      break;
    }
    case GeomAbs_Cylinder:
    {
      // Create Handle(Geom_CylindricalSurface) from gp_Cylinder
      Handle(Geom_CylindricalSurface) aGeomCyl = new Geom_CylindricalSurface(theSurface->Cylinder());
      myEvaluator                              = GeomGridEval_Cylinder(aGeomCyl);
      break;
    }
    case GeomAbs_Sphere:
    {
      // Create Handle(Geom_SphericalSurface) from gp_Sphere
      Handle(Geom_SphericalSurface) aGeomSphere = new Geom_SphericalSurface(theSurface->Sphere());
      myEvaluator                               = GeomGridEval_Sphere(aGeomSphere);
      break;
    }
    case GeomAbs_Cone:
    {
      // Create Handle(Geom_ConicalSurface) from gp_Cone
      Handle(Geom_ConicalSurface) aGeomCone = new Geom_ConicalSurface(theSurface->Cone());
      myEvaluator                           = GeomGridEval_Cone(aGeomCone);
      break;
    }
    case GeomAbs_Torus:
    {
      // Create Handle(Geom_ToroidalSurface) from gp_Torus
      Handle(Geom_ToroidalSurface) aGeomTorus = new Geom_ToroidalSurface(theSurface->Torus());
      myEvaluator                             = GeomGridEval_Torus(aGeomTorus);
      break;
    }
    case GeomAbs_BezierSurface:
    {
      myEvaluator = GeomGridEval_BezierSurface(theSurface->Bezier());
      break;
    }
    case GeomAbs_BSplineSurface:
    {
      myEvaluator = GeomGridEval_BSplineSurface(theSurface->BSpline());
      break;
    }
    case GeomAbs_OffsetSurface:
    {
      // Create Handle(Geom_OffsetSurface)
      // Note: Adaptor3d_Surface does not expose an OffsetSurface() method returning Geom_OffsetSurface directly
      // It exposes BasisSurface() and Offset value.
      // So we have to reconstruct it or rely on fallback if we can't reconstruct perfectly.
      // But GeomGridEval_OffsetSurface takes Handle(Geom_OffsetSurface).
      // So we must create a NEW Geom_OffsetSurface from the adaptor's data.
      // This might be expensive or lose some data if the adaptor was complex.
      // Ideally, we should only support Offset if we started from Geom_OffsetSurface (via handle init).
      // For pure adaptors, we might fallback or try to reconstruct.
      // Let's reconstruct for now as it fits the pattern.
      // But wait, BasisSurface() returns Adaptor3d_Surface. Geom_OffsetSurface needs Geom_Surface.
      // We can't easily convert Adaptor3d_Surface back to Geom_Surface without loss or complexity.
      // So for pure Adaptor initialization, OffsetSurface support is tricky.
      // We will fallback to OtherSurface for now in this switch case,
      // UNLESS we can safely cast. But we can't.
      myEvaluator = GeomGridEval_OtherSurface(theSurface->ShallowCopy());
      break;
    }
    default:
    {
      // Fallback: use OtherSurface.
      myEvaluator = GeomGridEval_OtherSurface(theSurface);
      break;
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::Initialize(const Handle(Geom_Surface)& theSurface)
{
  if (theSurface.IsNull())
  {
    myEvaluator = std::monostate{};
    mySurfaceType = GeomAbs_OtherSurface;
    return;
  }

  if (auto aPlane = Handle(Geom_Plane)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Plane;
    myEvaluator = GeomGridEval_Plane(aPlane);
  }
  else if (auto aCyl = Handle(Geom_CylindricalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Cylinder;
    myEvaluator = GeomGridEval_Cylinder(aCyl);
  }
  else if (auto aSphere = Handle(Geom_SphericalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Sphere;
    myEvaluator = GeomGridEval_Sphere(aSphere);
  }
  else if (auto aCone = Handle(Geom_ConicalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Cone;
    myEvaluator = GeomGridEval_Cone(aCone);
  }
  else if (auto aTorus = Handle(Geom_ToroidalSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_Torus;
    myEvaluator = GeomGridEval_Torus(aTorus);
  }
  else if (auto aBezier = Handle(Geom_BezierSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_BezierSurface;
    myEvaluator = GeomGridEval_BezierSurface(aBezier);
  }
  else if (auto aBSpline = Handle(Geom_BSplineSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_BSplineSurface;
    myEvaluator = GeomGridEval_BSplineSurface(aBSpline);
  }
  else if (auto anOffset = Handle(Geom_OffsetSurface)::DownCast(theSurface))
  {
    mySurfaceType = GeomAbs_OffsetSurface;
    myEvaluator = GeomGridEval_OffsetSurface(anOffset);
  }
  else
  {
    // Create adaptor for general surfaces (without copying geometry)
    Handle(GeomAdaptor_Surface) anAdaptor = new GeomAdaptor_Surface(theSurface);
    mySurfaceType = anAdaptor->GetType();
    myEvaluator = GeomGridEval_OtherSurface(anAdaptor);
  }
}

//==================================================================================================

void GeomGridEval_Surface::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

bool GeomGridEval_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

int GeomGridEval_Surface::NbUParams() const
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

int GeomGridEval_Surface::NbVParams() const
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

NCollection_Array2<gp_Pnt> GeomGridEval_Surface::EvaluateGrid() const
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

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Surface::EvaluateGridD1() const
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

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Surface::EvaluateGridD2() const
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
