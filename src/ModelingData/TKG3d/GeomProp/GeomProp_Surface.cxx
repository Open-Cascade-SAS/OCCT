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

#include <GeomAdaptor_Surface.hxx>

//==================================================================================================

void GeomProp_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  if (theSurface.IsKind(STANDARD_TYPE(GeomAdaptor_Surface)))
  {
    const auto& aGeomAdaptor = static_cast<const GeomAdaptor_Surface&>(theSurface);
    myAdaptor                = new GeomAdaptor_Surface(aGeomAdaptor);
    initFromAdaptor();
    return;
  }

  // For non-GeomAdaptor, set uninitialized.
  myAdaptor.Nullify();
  mySurfaceType = theSurface.GetType();
  myEvaluator.emplace<std::monostate>();
}

//==================================================================================================

void GeomProp_Surface::Initialize(const occ::handle<Geom_Surface>& theSurface)
{
  if (theSurface.IsNull())
  {
    myAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    mySurfaceType = GeomAbs_OtherSurface;
    return;
  }

  myAdaptor = new GeomAdaptor_Surface(theSurface);
  initFromAdaptor();
}

//==================================================================================================

void GeomProp_Surface::initFromAdaptor()
{
  mySurfaceType                        = myAdaptor->GetType();
  const GeomAdaptor_Surface* aPtr = myAdaptor.get();

  switch (mySurfaceType)
  {
    case GeomAbs_Plane:
      myEvaluator.emplace<GeomProp_Plane>(aPtr);
      break;
    case GeomAbs_Cylinder:
      myEvaluator.emplace<GeomProp_Cylinder>(aPtr);
      break;
    case GeomAbs_Cone:
      myEvaluator.emplace<GeomProp_Cone>(aPtr);
      break;
    case GeomAbs_Sphere:
      myEvaluator.emplace<GeomProp_Sphere>(aPtr);
      break;
    case GeomAbs_Torus:
      myEvaluator.emplace<GeomProp_Torus>(aPtr);
      break;
    case GeomAbs_BezierSurface:
      myEvaluator.emplace<GeomProp_BezierSurface>(aPtr);
      break;
    case GeomAbs_BSplineSurface:
      myEvaluator.emplace<GeomProp_BSplineSurface>(aPtr);
      break;
    case GeomAbs_SurfaceOfRevolution:
      myEvaluator.emplace<GeomProp_SurfaceOfRevolution>(aPtr);
      break;
    case GeomAbs_SurfaceOfExtrusion:
      myEvaluator.emplace<GeomProp_SurfaceOfExtrusion>(aPtr);
      break;
    case GeomAbs_OffsetSurface:
      myEvaluator.emplace<GeomProp_OffsetSurface>(aPtr);
      break;
    default:
      myEvaluator.emplace<GeomProp_OtherSurface>(aPtr);
      break;
  }
}

//==================================================================================================

bool GeomProp_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

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

//==================================================================================================

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

//==================================================================================================

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
