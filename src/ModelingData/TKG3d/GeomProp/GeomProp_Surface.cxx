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

#include <GeomProp_Surface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_TransformedSurface.hxx>

//=================================================================================================

GeomProp_Surface::GeomProp_Surface(const Adaptor3d_Surface& theSurface)
    : myEvaluator(std::monostate{}),
      mySurfaceType(GeomAbs_OtherSurface)
{
  initialization(theSurface);
}

//=================================================================================================

GeomProp_Surface::GeomProp_Surface(const occ::handle<Geom_Surface>& theSurface)
    : myEvaluator(std::monostate{}),
      mySurfaceType(GeomAbs_OtherSurface)
{
  initialization(theSurface);
}

//=================================================================================================

void GeomProp_Surface::initialization(const Adaptor3d_Surface& theSurface)
{
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_TransformedSurface)))
  {
    // Extract the transformed adaptor (carries both geometry and domain) and re-initialize.
    const GeomAdaptor_TransformedSurface& aTransformed =
      static_cast<const GeomAdaptor_TransformedSurface&>(theSurface);
    initialization(aTransformed.AdaptorSurfaceTransformed());
    return;
  }

  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_Surface)))
  {
    const GeomAdaptor_Surface& aGeomAdaptor = static_cast<const GeomAdaptor_Surface&>(theSurface);
    if (!aGeomAdaptor.Surface().IsNull())
    {
      // Dispatch specialized evaluators directly from the GeomAdaptor.
      myOwnedAdaptor.Nullify();
      mySurfaceType = aGeomAdaptor.GetType();
      switch (mySurfaceType)
      {
        case GeomAbs_Plane:
          myEvaluator.emplace<GeomProp_Plane>(&aGeomAdaptor);
          break;
        case GeomAbs_Cylinder:
          myEvaluator.emplace<GeomProp_Cylinder>(&aGeomAdaptor);
          break;
        case GeomAbs_Cone:
          myEvaluator.emplace<GeomProp_Cone>(&aGeomAdaptor);
          break;
        case GeomAbs_Sphere:
          myEvaluator.emplace<GeomProp_Sphere>(&aGeomAdaptor);
          break;
        case GeomAbs_Torus:
          myEvaluator.emplace<GeomProp_Torus>(&aGeomAdaptor);
          break;
        case GeomAbs_BezierSurface:
          myEvaluator.emplace<GeomProp_BezierSurface>(&aGeomAdaptor);
          break;
        case GeomAbs_BSplineSurface:
          myEvaluator.emplace<GeomProp_BSplineSurface>(&aGeomAdaptor);
          break;
        case GeomAbs_SurfaceOfRevolution:
          myEvaluator.emplace<GeomProp_SurfaceOfRevolution>(&aGeomAdaptor);
          break;
        case GeomAbs_SurfaceOfExtrusion:
          myEvaluator.emplace<GeomProp_SurfaceOfExtrusion>(&aGeomAdaptor);
          break;
        case GeomAbs_OffsetSurface:
          myEvaluator.emplace<GeomProp_OffsetSurface>(&aGeomAdaptor);
          break;
        default:
          myEvaluator.emplace<GeomProp_OtherSurface>(&theSurface);
          break;
      }
      return;
    }
  }

  // Non-Geom adaptor or empty surface handle: use OtherSurface with adaptor pointer.
  myOwnedAdaptor.Nullify();
  mySurfaceType = theSurface.GetType();
  myEvaluator.emplace<GeomProp_OtherSurface>(&theSurface);
}

//=================================================================================================

void GeomProp_Surface::initialization(const occ::handle<Geom_Surface>& theSurface)
{
  if (theSurface.IsNull())
  {
    myOwnedAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    mySurfaceType = GeomAbs_OtherSurface;
    return;
  }

  // No adaptor creation for the handle path.
  myOwnedAdaptor.Nullify();

  // Unwrap RectangularTrimmedSurface to basis surface + optional SurfaceDomain.
  occ::handle<Geom_Surface>               aBasis = theSurface;
  std::optional<GeomProp::SurfaceDomain> aDomain;
  if (aBasis->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    double aU1, aU2, aV1, aV2;
    occ::down_cast<Geom_RectangularTrimmedSurface>(aBasis)->Bounds(aU1, aU2, aV1, aV2);
    aDomain = GeomProp::SurfaceDomain{aU1, aU2, aV1, aV2};
    aBasis  = occ::down_cast<Geom_RectangularTrimmedSurface>(aBasis)->BasisSurface();
  }

  // Type detection using DynamicType() - extracted once to avoid repeated virtual calls.
  const occ::handle<Standard_Type>& aType = aBasis->DynamicType();
  if (aType == STANDARD_TYPE(Geom_Plane))
  {
    mySurfaceType = GeomAbs_Plane;
    myEvaluator.emplace<GeomProp_Plane>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    mySurfaceType = GeomAbs_Cylinder;
    myEvaluator.emplace<GeomProp_Cylinder>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    mySurfaceType = GeomAbs_Cone;
    myEvaluator.emplace<GeomProp_Cone>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    mySurfaceType = GeomAbs_Sphere;
    myEvaluator.emplace<GeomProp_Sphere>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    mySurfaceType = GeomAbs_Torus;
    myEvaluator.emplace<GeomProp_Torus>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_BezierSurface))
  {
    mySurfaceType = GeomAbs_BezierSurface;
    myEvaluator.emplace<GeomProp_BezierSurface>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    mySurfaceType = GeomAbs_BSplineSurface;
    myEvaluator.emplace<GeomProp_BSplineSurface>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    mySurfaceType = GeomAbs_SurfaceOfRevolution;
    myEvaluator.emplace<GeomProp_SurfaceOfRevolution>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    mySurfaceType = GeomAbs_SurfaceOfExtrusion;
    myEvaluator.emplace<GeomProp_SurfaceOfExtrusion>(aBasis, aDomain);
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetSurface))
  {
    mySurfaceType = GeomAbs_OffsetSurface;
    myEvaluator.emplace<GeomProp_OffsetSurface>(aBasis, aDomain);
  }
  else
  {
    mySurfaceType = GeomAbs_OtherSurface;
    myEvaluator.emplace<GeomProp_OtherSurface>(aBasis, aDomain);
  }
}

//=================================================================================================

const Adaptor3d_Surface* GeomProp_Surface::Adaptor() const
{
  if (!myOwnedAdaptor.IsNull())
  {
    return myOwnedAdaptor.get();
  }
  return nullptr;
}

//=================================================================================================

GeomProp::SurfaceNormalResult GeomProp_Surface::Normal(const double theU,
                                                       const double theV,
                                                       const double theTol) const
{
  return std::visit(
    [theU, theV, theTol](const auto& theEval) -> GeomProp::SurfaceNormalResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.Normal(theU, theV, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

GeomProp::SurfaceCurvatureResult GeomProp_Surface::Curvatures(const double theU,
                                                              const double theV,
                                                              const double theTol) const
{
  return std::visit(
    [theU, theV, theTol](const auto& theEval) -> GeomProp::SurfaceCurvatureResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {};
      }
      else
      {
        return theEval.Curvatures(theU, theV, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

GeomProp::MeanGaussianResult GeomProp_Surface::MeanGaussian(const double theU,
                                                            const double theV,
                                                            const double theTol) const
{
  return std::visit(
    [theU, theV, theTol](const auto& theEval) -> GeomProp::MeanGaussianResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {};
      }
      else
      {
        return theEval.MeanGaussian(theU, theV, theTol);
      }
    },
    myEvaluator);
}
