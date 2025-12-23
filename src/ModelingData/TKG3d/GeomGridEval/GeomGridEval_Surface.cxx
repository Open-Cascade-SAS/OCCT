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
#include <GeomAdaptor_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>

namespace
{

//! Extracts basis surface from potentially nested RectangularTrimmedSurface wrappers.
//! @param theSurface input surface (may be RectangularTrimmedSurface or any other)
//! @return the underlying basis surface, or theSurface if not a RectangularTrimmedSurface
Handle(Geom_Surface) ExtractBasisSurface(const Handle(Geom_Surface)& theSurface)
{
  Handle(Geom_Surface) aResult = theSurface;
  while (auto aTrimmed = Handle(Geom_RectangularTrimmedSurface)::DownCast(aResult))
  {
    aResult = aTrimmed->BasisSurface();
  }
  return aResult;
}

//! Creates Geom_Surface from adaptor's elementary surface type.
//! @param theSurface the adaptor to extract geometry from
//! @return Geom_Surface handle, or null if type is not elementary
Handle(Geom_Surface) CreateGeomSurfaceFromAdaptor(const Adaptor3d_Surface& theSurface)
{
  switch (theSurface.GetType())
  {
    case GeomAbs_Plane:
      return new Geom_Plane(theSurface.Plane());
    case GeomAbs_Cylinder:
      return new Geom_CylindricalSurface(theSurface.Cylinder());
    case GeomAbs_Cone:
      return new Geom_ConicalSurface(theSurface.Cone());
    case GeomAbs_Sphere:
      return new Geom_SphericalSurface(theSurface.Sphere());
    case GeomAbs_Torus:
      return new Geom_ToroidalSurface(theSurface.Torus());
    default:
      return Handle(Geom_Surface)();
  }
}

} // namespace

//==================================================================================================

void GeomGridEval_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  // Reset transformation
  myTrsf.reset();

  // Check for GeomAdaptor_TransformedSurface (includes BRepAdaptor_Surface)
  // to extract transformation and underlying geometry
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_TransformedSurface)))
  {
    const auto&    aTransformed = static_cast<const GeomAdaptor_TransformedSurface&>(theSurface);
    const gp_Trsf& aTrsf        = aTransformed.Trsf();

    // Only store transformation if it's not identity
    if (aTrsf.Form() != gp_Identity)
    {
      myTrsf = aTrsf;
    }

    // Initialize with the underlying Geom_Surface
    Initialize(aTransformed.GeomSurface());
    return;
  }

  // Check for SurfaceOfRevolution adaptor - may not have stored Geom_Surface
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_SurfaceOfRevolution)))
  {
    const auto& aRevAdaptor = static_cast<const GeomAdaptor_SurfaceOfRevolution&>(theSurface);

    // First try to get elementary surface type (Plane, Cylinder, Cone, Sphere, Torus)
    Handle(Geom_Surface) aGeomSurf = CreateGeomSurfaceFromAdaptor(aRevAdaptor);
    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }

    // Try the stored Surface() handle
    aGeomSurf = aRevAdaptor.Surface();
    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }

    // No geometry available - use adaptor directly as fallback
    mySurfaceType = theSurface.GetType();
    myEvaluator.emplace<GeomGridEval_OtherSurface>(theSurface);
    return;
  }

  // Check for SurfaceOfExtrusion adaptor - may not have stored Geom_Surface
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_SurfaceOfLinearExtrusion)))
  {
    const auto& aExtAdaptor = static_cast<const GeomAdaptor_SurfaceOfLinearExtrusion&>(theSurface);

    // First try to get elementary surface type (Plane, Cylinder, Cone, Sphere, Torus)
    Handle(Geom_Surface) aGeomSurf = CreateGeomSurfaceFromAdaptor(aExtAdaptor);
    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }

    // Try the stored Surface() handle
    aGeomSurf = aExtAdaptor.Surface();
    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }

    // No geometry available - use adaptor directly as fallback
    mySurfaceType = theSurface.GetType();
    myEvaluator.emplace<GeomGridEval_OtherSurface>(theSurface);
    return;
  }

  // Check for plain GeomAdaptor_Surface (without transformation)
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_Surface)))
  {
    const auto&          aGeomAdaptor = static_cast<const GeomAdaptor_Surface&>(theSurface);
    Handle(Geom_Surface) aGeomSurf    = aGeomAdaptor.Surface();

    // If Surface() is null, try to create from elementary type
    if (aGeomSurf.IsNull())
    {
      aGeomSurf = CreateGeomSurfaceFromAdaptor(aGeomAdaptor);
    }

    if (!aGeomSurf.IsNull())
    {
      Initialize(aGeomSurf);
      return;
    }

    // Surface() is null and not elementary - use adaptor directly
    mySurfaceType = theSurface.GetType();
    myEvaluator.emplace<GeomGridEval_OtherSurface>(theSurface);
    return;
  }

  // For non-GeomAdaptor adaptors, try to create Geom_Surface from elementary type first
  Handle(Geom_Surface) aGeomSurf = CreateGeomSurfaceFromAdaptor(theSurface);
  if (!aGeomSurf.IsNull())
  {
    Initialize(aGeomSurf);
    return;
  }

  // No way to get Geom_Surface - use adaptor directly as fallback
  mySurfaceType = theSurface.GetType();
  myEvaluator.emplace<GeomGridEval_OtherSurface>(theSurface);
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

  // Extract basis surface from potentially nested RectangularTrimmedSurface wrappers
  Handle(Geom_Surface) aBasisSurf = ExtractBasisSurface(theSurface);

  if (auto aPlane = Handle(Geom_Plane)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_Plane;
    myEvaluator.emplace<GeomGridEval_Plane>(aPlane);
  }
  else if (auto aCyl = Handle(Geom_CylindricalSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_Cylinder;
    myEvaluator.emplace<GeomGridEval_Cylinder>(aCyl);
  }
  else if (auto aSphere = Handle(Geom_SphericalSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_Sphere;
    myEvaluator.emplace<GeomGridEval_Sphere>(aSphere);
  }
  else if (auto aCone = Handle(Geom_ConicalSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_Cone;
    myEvaluator.emplace<GeomGridEval_Cone>(aCone);
  }
  else if (auto aTorus = Handle(Geom_ToroidalSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_Torus;
    myEvaluator.emplace<GeomGridEval_Torus>(aTorus);
  }
  else if (auto aBezier = Handle(Geom_BezierSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_BezierSurface;
    myEvaluator.emplace<GeomGridEval_BezierSurface>(aBezier);
  }
  else if (auto aBSpline = Handle(Geom_BSplineSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_BSplineSurface;
    myEvaluator.emplace<GeomGridEval_BSplineSurface>(aBSpline);
  }
  else if (auto anOffset = Handle(Geom_OffsetSurface)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_OffsetSurface;
    myEvaluator.emplace<GeomGridEval_OffsetSurface>(anOffset);
  }
  else if (auto aRevolution = Handle(Geom_SurfaceOfRevolution)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_SurfaceOfRevolution;
    myEvaluator.emplace<GeomGridEval_SurfaceOfRevolution>(aRevolution);
  }
  else if (auto anExtrusion = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aBasisSurf))
  {
    mySurfaceType = GeomAbs_SurfaceOfExtrusion;
    myEvaluator.emplace<GeomGridEval_SurfaceOfExtrusion>(anExtrusion);
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
  NCollection_Array2<gp_Pnt> aResult = std::visit(
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

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_Surface::EvaluateGridD1() const
{
  NCollection_Array2<GeomGridEval::SurfD1> aResult = std::visit(
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

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_Surface::EvaluateGridD2() const
{
  NCollection_Array2<GeomGridEval::SurfD2> aResult = std::visit(
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

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_Surface::EvaluateGridD3() const
{
  NCollection_Array2<GeomGridEval::SurfD3> aResult = std::visit(
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

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_Surface::EvaluateGridDN(int theNU, int theNV) const
{
  NCollection_Array2<gp_Vec> aResult = std::visit(
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

  if (myTrsf.has_value())
  {
    applyTransformation(aResult);
  }

  return aResult;
}

//==================================================================================================

void GeomGridEval_Surface::applyTransformation(NCollection_Array2<gp_Pnt>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int aUIdx = theGrid.LowerRow(); aUIdx <= theGrid.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = theGrid.LowerCol(); aVIdx <= theGrid.UpperCol(); ++aVIdx)
    {
      theGrid.ChangeValue(aUIdx, aVIdx).Transform(aTrsf);
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::applyTransformation(
  NCollection_Array2<GeomGridEval::SurfD1>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int aUIdx = theGrid.LowerRow(); aUIdx <= theGrid.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = theGrid.LowerCol(); aVIdx <= theGrid.UpperCol(); ++aVIdx)
    {
      GeomGridEval::SurfD1& aVal = theGrid.ChangeValue(aUIdx, aVIdx);
      aVal.Point.Transform(aTrsf);
      aVal.D1U.Transform(aTrsf);
      aVal.D1V.Transform(aTrsf);
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::applyTransformation(
  NCollection_Array2<GeomGridEval::SurfD2>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int aUIdx = theGrid.LowerRow(); aUIdx <= theGrid.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = theGrid.LowerCol(); aVIdx <= theGrid.UpperCol(); ++aVIdx)
    {
      GeomGridEval::SurfD2& aVal = theGrid.ChangeValue(aUIdx, aVIdx);
      aVal.Point.Transform(aTrsf);
      aVal.D1U.Transform(aTrsf);
      aVal.D1V.Transform(aTrsf);
      aVal.D2U.Transform(aTrsf);
      aVal.D2V.Transform(aTrsf);
      aVal.D2UV.Transform(aTrsf);
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::applyTransformation(
  NCollection_Array2<GeomGridEval::SurfD3>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int aUIdx = theGrid.LowerRow(); aUIdx <= theGrid.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = theGrid.LowerCol(); aVIdx <= theGrid.UpperCol(); ++aVIdx)
    {
      GeomGridEval::SurfD3& aVal = theGrid.ChangeValue(aUIdx, aVIdx);
      aVal.Point.Transform(aTrsf);
      aVal.D1U.Transform(aTrsf);
      aVal.D1V.Transform(aTrsf);
      aVal.D2U.Transform(aTrsf);
      aVal.D2V.Transform(aTrsf);
      aVal.D2UV.Transform(aTrsf);
      aVal.D3U.Transform(aTrsf);
      aVal.D3V.Transform(aTrsf);
      aVal.D3UUV.Transform(aTrsf);
      aVal.D3UVV.Transform(aTrsf);
    }
  }
}

//==================================================================================================

void GeomGridEval_Surface::applyTransformation(NCollection_Array2<gp_Vec>& theGrid) const
{
  if (!myTrsf.has_value() || theGrid.IsEmpty())
  {
    return;
  }

  const gp_Trsf& aTrsf = myTrsf.value();
  for (int aUIdx = theGrid.LowerRow(); aUIdx <= theGrid.UpperRow(); ++aUIdx)
  {
    for (int aVIdx = theGrid.LowerCol(); aVIdx <= theGrid.UpperCol(); ++aVIdx)
    {
      theGrid.ChangeValue(aUIdx, aVIdx).Transform(aTrsf);
    }
  }
}
