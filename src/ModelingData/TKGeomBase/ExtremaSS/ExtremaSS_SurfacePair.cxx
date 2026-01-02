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

#include <ExtremaSS_SurfacePair.hxx>

#include <GeomAbs_SurfaceType.hxx>

namespace
{
//! Static result for uninitialized state.
static const ExtremaSS::Result THE_NOT_DONE_RESULT = []() {
  ExtremaSS::Result aResult;
  aResult.Status = ExtremaSS::Status::NotDone;
  return aResult;
}();

//! Surface type index for ordering.
enum class SurfaceTypeIndex : int
{
  Plane    = 0,
  Cylinder = 1,
  Cone     = 2,
  Sphere   = 3,
  Torus    = 4,
  Other    = 5
};

//! Get surface type index from adaptor.
SurfaceTypeIndex getSurfaceTypeIndex(const GeomAdaptor_Surface& theSurface)
{
  switch (theSurface.GetType())
  {
    case GeomAbs_Plane:
      return SurfaceTypeIndex::Plane;
    case GeomAbs_Cylinder:
      return SurfaceTypeIndex::Cylinder;
    case GeomAbs_Cone:
      return SurfaceTypeIndex::Cone;
    case GeomAbs_Sphere:
      return SurfaceTypeIndex::Sphere;
    case GeomAbs_Torus:
      return SurfaceTypeIndex::Torus;
    default:
      return SurfaceTypeIndex::Other;
  }
}

} // namespace

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_Surface& theSurface1,
                                             const GeomAdaptor_Surface& theSurface2)
    : mySwapped(false)
{
  // Create domain from surface bounds
  ExtremaSS::Domain4D aDomain;
  aDomain.Domain1 =
    ExtremaSS::Domain2D(theSurface1.FirstUParameter(), theSurface1.LastUParameter(),
                        theSurface1.FirstVParameter(), theSurface1.LastVParameter());
  aDomain.Domain2 =
    ExtremaSS::Domain2D(theSurface2.FirstUParameter(), theSurface2.LastUParameter(),
                        theSurface2.FirstVParameter(), theSurface2.LastVParameter());

  initializeEvaluator(theSurface1, theSurface2, aDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_Surface& theSurface1,
                                             const GeomAdaptor_Surface& theSurface2,
                                             const ExtremaSS::Domain4D& theDomain)
    : mySwapped(false)
{
  initializeEvaluator(theSurface1, theSurface2, theDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_Surface&            theSurface1,
                                             const GeomAdaptor_TransformedSurface& theSurface2)
    : mySwapped(false)
{
  // Create domain from surface bounds
  ExtremaSS::Domain4D aDomain;
  aDomain.Domain1 =
    ExtremaSS::Domain2D(theSurface1.FirstUParameter(), theSurface1.LastUParameter(),
                        theSurface1.FirstVParameter(), theSurface1.LastVParameter());
  aDomain.Domain2 =
    ExtremaSS::Domain2D(theSurface2.FirstUParameter(), theSurface2.LastUParameter(),
                        theSurface2.FirstVParameter(), theSurface2.LastVParameter());

  // Use the underlying surface from TransformedSurface
  initializeEvaluator(theSurface1, theSurface2.Surface(), aDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_Surface&            theSurface1,
                                             const GeomAdaptor_TransformedSurface& theSurface2,
                                             const ExtremaSS::Domain4D&            theDomain)
    : mySwapped(false)
{
  initializeEvaluator(theSurface1, theSurface2.Surface(), theDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                             const GeomAdaptor_Surface&            theSurface2)
    : mySwapped(false)
{
  // Create domain from surface bounds
  ExtremaSS::Domain4D aDomain;
  aDomain.Domain1 =
    ExtremaSS::Domain2D(theSurface1.FirstUParameter(), theSurface1.LastUParameter(),
                        theSurface1.FirstVParameter(), theSurface1.LastVParameter());
  aDomain.Domain2 =
    ExtremaSS::Domain2D(theSurface2.FirstUParameter(), theSurface2.LastUParameter(),
                        theSurface2.FirstVParameter(), theSurface2.LastVParameter());

  // Use the underlying surface from TransformedSurface
  initializeEvaluator(theSurface1.Surface(), theSurface2, aDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                             const GeomAdaptor_Surface&            theSurface2,
                                             const ExtremaSS::Domain4D&            theDomain)
    : mySwapped(false)
{
  initializeEvaluator(theSurface1.Surface(), theSurface2, theDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                             const GeomAdaptor_TransformedSurface& theSurface2)
    : mySwapped(false)
{
  // Create domain from surface bounds
  ExtremaSS::Domain4D aDomain;
  aDomain.Domain1 =
    ExtremaSS::Domain2D(theSurface1.FirstUParameter(), theSurface1.LastUParameter(),
                        theSurface1.FirstVParameter(), theSurface1.LastVParameter());
  aDomain.Domain2 =
    ExtremaSS::Domain2D(theSurface2.FirstUParameter(), theSurface2.LastUParameter(),
                        theSurface2.FirstVParameter(), theSurface2.LastVParameter());

  // Use the underlying surfaces from TransformedSurfaces
  initializeEvaluator(theSurface1.Surface(), theSurface2.Surface(), aDomain);
}

//==================================================================================================

ExtremaSS_SurfacePair::ExtremaSS_SurfacePair(const GeomAdaptor_TransformedSurface& theSurface1,
                                             const GeomAdaptor_TransformedSurface& theSurface2,
                                             const ExtremaSS::Domain4D&            theDomain)
    : mySwapped(false)
{
  initializeEvaluator(theSurface1.Surface(), theSurface2.Surface(), theDomain);
}

//==================================================================================================

void ExtremaSS_SurfacePair::initializeEvaluator(const GeomAdaptor_Surface& theSurface1,
                                                const GeomAdaptor_Surface& theSurface2,
                                                const ExtremaSS::Domain4D& theDomain)
{
  const SurfaceTypeIndex aType1 = getSurfaceTypeIndex(theSurface1);
  const SurfaceTypeIndex aType2 = getSurfaceTypeIndex(theSurface2);

  // Ensure canonical ordering: lower type index first
  const bool aNeedSwap                = static_cast<int>(aType1) > static_cast<int>(aType2);
  mySwapped                           = aNeedSwap;
  const Adaptor3d_Surface& aSurf1     = aNeedSwap ? theSurface2 : theSurface1;
  const Adaptor3d_Surface& aSurf2     = aNeedSwap ? theSurface1 : theSurface2;
  const SurfaceTypeIndex   aTypeFirst = aNeedSwap ? aType2 : aType1;
  const SurfaceTypeIndex   aTypeSecond = aNeedSwap ? aType1 : aType2;
  const ExtremaSS::Domain4D aDomain =
    aNeedSwap ? ExtremaSS::Domain4D(theDomain.Domain2, theDomain.Domain1) : theDomain;

  // Dispatch based on surface type pair
  switch (aTypeFirst)
  {
    case SurfaceTypeIndex::Plane:
      switch (aTypeSecond)
      {
        case SurfaceTypeIndex::Plane:
          myEvaluator = ExtremaSS_PlanePlane(aSurf1.Plane(), aSurf2.Plane(), aDomain);
          return;
        case SurfaceTypeIndex::Cylinder:
          myEvaluator = ExtremaSS_PlaneCylinder(aSurf1.Plane(), aSurf2.Cylinder(), aDomain);
          return;
        case SurfaceTypeIndex::Cone:
          myEvaluator = ExtremaSS_PlaneCone(aSurf1.Plane(), aSurf2.Cone(), aDomain);
          return;
        case SurfaceTypeIndex::Sphere:
          myEvaluator = ExtremaSS_PlaneSphere(aSurf1.Plane(), aSurf2.Sphere(), aDomain);
          return;
        case SurfaceTypeIndex::Torus:
          // myEvaluator = ExtremaSS_PlaneTorus(aSurf1.Plane(), aSurf2.Torus(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        default:
          break;
      }
      break;

    case SurfaceTypeIndex::Cylinder:
      switch (aTypeSecond)
      {
        case SurfaceTypeIndex::Cylinder:
          // myEvaluator = ExtremaSS_CylinderCylinder(aSurf1.Cylinder(), aSurf2.Cylinder(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        case SurfaceTypeIndex::Cone:
          // myEvaluator = ExtremaSS_CylinderCone(aSurf1.Cylinder(), aSurf2.Cone(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        case SurfaceTypeIndex::Sphere:
          myEvaluator = ExtremaSS_CylinderSphere(aSurf1.Cylinder(), aSurf2.Sphere(), aDomain);
          return;
        case SurfaceTypeIndex::Torus:
          // myEvaluator = ExtremaSS_CylinderTorus(aSurf1.Cylinder(), aSurf2.Torus(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        default:
          break;
      }
      break;

    case SurfaceTypeIndex::Cone:
      switch (aTypeSecond)
      {
        case SurfaceTypeIndex::Cone:
          // myEvaluator = ExtremaSS_ConeCone(aSurf1.Cone(), aSurf2.Cone(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        case SurfaceTypeIndex::Sphere:
          // myEvaluator = ExtremaSS_ConeSphere(aSurf1.Cone(), aSurf2.Sphere(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        case SurfaceTypeIndex::Torus:
          // myEvaluator = ExtremaSS_ConeTorus(aSurf1.Cone(), aSurf2.Torus(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        default:
          break;
      }
      break;

    case SurfaceTypeIndex::Sphere:
      switch (aTypeSecond)
      {
        case SurfaceTypeIndex::Sphere:
          myEvaluator = ExtremaSS_SphereSphere(aSurf1.Sphere(), aSurf2.Sphere(), aDomain);
          return;
        case SurfaceTypeIndex::Torus:
          // myEvaluator = ExtremaSS_SphereTorus(aSurf1.Sphere(), aSurf2.Torus(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        default:
          break;
      }
      break;

    case SurfaceTypeIndex::Torus:
      switch (aTypeSecond)
      {
        case SurfaceTypeIndex::Torus:
          // myEvaluator = ExtremaSS_TorusTorus(aSurf1.Torus(), aSurf2.Torus(), aDomain);
          myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
          return;
        default:
          break;
      }
      break;

    default:
      break;
  }

  // Fallback to generic pair
  myEvaluator = ExtremaSS_GenericPair(aSurf1, aSurf2, aDomain);
}

//==================================================================================================

bool ExtremaSS_SurfacePair::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_SurfacePair::Perform(double                theTol,
                                                        ExtremaSS::SearchMode theMode) const
{
  const ExtremaSS::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  std::visit(
    [&](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        aResultPtr = &theEval.Perform(theTol, theMode);
      }
    },
    myEvaluator);

  return *aResultPtr;
}

//==================================================================================================

const ExtremaSS::Result& ExtremaSS_SurfacePair::PerformWithBoundary(
  double                theTol,
  ExtremaSS::SearchMode theMode) const
{
  const ExtremaSS::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  std::visit(
    [&](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        aResultPtr = &theEval.PerformWithBoundary(theTol, theMode);
      }
    },
    myEvaluator);

  return *aResultPtr;
}
