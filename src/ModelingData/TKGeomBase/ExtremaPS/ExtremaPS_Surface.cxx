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

#include <ExtremaPS_Surface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface()
    : myEvaluator(std::monostate{})
{
}

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface(const Adaptor3d_Surface& theSurface)
    : myEvaluator(std::monostate{})
{
  myDomain = ExtremaPS::Domain2D(theSurface.FirstUParameter(),
                                  theSurface.LastUParameter(),
                                  theSurface.FirstVParameter(),
                                  theSurface.LastVParameter());
  initializeEvaluator(theSurface, myDomain);
}

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface(const Adaptor3d_Surface&   theSurface,
                                      const ExtremaPS::Domain2D& theDomain)
    : myEvaluator(std::monostate{}),
      myDomain(theDomain)
{
  initializeEvaluator(theSurface, myDomain);
}

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface)
    : myEvaluator(std::monostate{})
{
  myDomain = ExtremaPS::Domain2D(theSurface.FirstUParameter(),
                                  theSurface.LastUParameter(),
                                  theSurface.FirstVParameter(),
                                  theSurface.LastVParameter());
  initializeEvaluator(theSurface, myDomain);
}

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface,
                                      const ExtremaPS::Domain2D& theDomain)
    : myEvaluator(std::monostate{}),
      myDomain(theDomain)
{
  initializeEvaluator(theSurface, myDomain);
}

//==================================================================================================

void ExtremaPS_Surface::initializeEvaluator(const Adaptor3d_Surface&   theSurface,
                                             const ExtremaPS::Domain2D& theDomain)
{
  const GeomAbs_SurfaceType aSurfType = theSurface.GetType();

  switch (aSurfType)
  {
    case GeomAbs_Plane:
      myEvaluator = ExtremaPS_Plane(theSurface.Plane(), theDomain);
      break;

    case GeomAbs_Cylinder:
      myEvaluator = ExtremaPS_Cylinder(theSurface.Cylinder(), theDomain);
      break;

    case GeomAbs_Cone:
      myEvaluator = ExtremaPS_Cone(theSurface.Cone(), theDomain);
      break;

    case GeomAbs_Sphere:
      myEvaluator = ExtremaPS_Sphere(theSurface.Sphere(), theDomain);
      break;

    case GeomAbs_Torus:
      myEvaluator = ExtremaPS_Torus(theSurface.Torus(), theDomain);
      break;

    case GeomAbs_BezierSurface:
      myEvaluator = ExtremaPS_BezierSurface(theSurface.Bezier(), theDomain);
      break;

    case GeomAbs_BSplineSurface:
      myEvaluator = ExtremaPS_BSplineSurface(theSurface.BSpline(), theDomain);
      break;

    case GeomAbs_OffsetSurface:
    {
      // For offset surfaces, we need to extract the underlying Geom_OffsetSurface
      // Try downcasting from GeomAdaptor_Surface
      const GeomAdaptor_Surface* aGeomAdaptor =
        dynamic_cast<const GeomAdaptor_Surface*>(&theSurface);
      if (aGeomAdaptor != nullptr)
      {
        Handle(Geom_OffsetSurface) anOffsetSurf =
          Handle(Geom_OffsetSurface)::DownCast(aGeomAdaptor->Surface());
        if (!anOffsetSurf.IsNull())
        {
          myEvaluator = ExtremaPS_OffsetSurface(anOffsetSurf, theDomain);
          break;
        }
      }
      // Fallback to OtherSurface if we can't get the offset surface handle
      if (const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(&theSurface))
      {
        myEvaluator = ExtremaPS_OtherSurface(aGA->Surface(), theDomain);
      }
      break;
    }

    default:
    {
      // For SurfaceOfRevolution, SurfaceOfExtrusion, and other types
      const GeomAdaptor_Surface* aGeomAdaptor =
        dynamic_cast<const GeomAdaptor_Surface*>(&theSurface);
      if (aGeomAdaptor != nullptr && !aGeomAdaptor->Surface().IsNull())
      {
        myEvaluator = ExtremaPS_OtherSurface(aGeomAdaptor->Surface(), theDomain);
      }
      break;
    }
  }
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::Perform(const gp_Pnt& theP, double theTol) const
{
  return std::visit(
    [&](const auto& theEval) -> ExtremaPS::Result {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        ExtremaPS::Result aResult;
        aResult.Status = ExtremaPS::Status::NotDone;
        return aResult;
      }
      else
      {
        return theEval.Perform(theP, theTol, mySearchMode);
      }
    },
    myEvaluator);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::PerformWithBoundary(const gp_Pnt& theP, double theTol) const
{
  return std::visit(
    [&](const auto& theEval) -> ExtremaPS::Result {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        ExtremaPS::Result aResult;
        aResult.Status = ExtremaPS::Status::NotDone;
        return aResult;
      }
      else
      {
        return theEval.PerformWithBoundary(theP, theTol, mySearchMode);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool ExtremaPS_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}
