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
  Initialize(theSurface);
}

//==================================================================================================

ExtremaPS_Surface::ExtremaPS_Surface(const GeomAdaptor_Surface& theSurface)
    : myEvaluator(std::monostate{})
{
  Initialize(theSurface);
}

//==================================================================================================

void ExtremaPS_Surface::Initialize(const Adaptor3d_Surface& theSurface)
{
  myDomain = ExtremaPS::Domain2D(theSurface.FirstUParameter(),
                                  theSurface.LastUParameter(),
                                  theSurface.FirstVParameter(),
                                  theSurface.LastVParameter());

  initializeEvaluator(theSurface);
}

//==================================================================================================

void ExtremaPS_Surface::Initialize(const GeomAdaptor_Surface& theSurface)
{
  myDomain = ExtremaPS::Domain2D(theSurface.FirstUParameter(),
                                  theSurface.LastUParameter(),
                                  theSurface.FirstVParameter(),
                                  theSurface.LastVParameter());

  initializeEvaluator(theSurface);
}

//==================================================================================================

void ExtremaPS_Surface::initializeEvaluator(const Adaptor3d_Surface& theSurface)
{
  const GeomAbs_SurfaceType aSurfType = theSurface.GetType();

  switch (aSurfType)
  {
    case GeomAbs_Plane:
      myEvaluator = ExtremaPS_Plane(theSurface.Plane());
      break;

    case GeomAbs_Cylinder:
      myEvaluator = ExtremaPS_Cylinder(theSurface.Cylinder());
      break;

    case GeomAbs_Cone:
      myEvaluator = ExtremaPS_Cone(theSurface.Cone());
      break;

    case GeomAbs_Sphere:
      myEvaluator = ExtremaPS_Sphere(theSurface.Sphere());
      break;

    case GeomAbs_Torus:
      myEvaluator = ExtremaPS_Torus(theSurface.Torus());
      break;

    case GeomAbs_BezierSurface:
      myEvaluator = ExtremaPS_BezierSurface(theSurface.Bezier());
      break;

    case GeomAbs_BSplineSurface:
      myEvaluator = ExtremaPS_BSplineSurface(theSurface.BSpline());
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
          myEvaluator = ExtremaPS_OffsetSurface(anOffsetSurf);
          break;
        }
      }
      // Fallback to OtherSurface if we can't get the offset surface handle
      if (const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(&theSurface))
      {
        myEvaluator = ExtremaPS_OtherSurface(aGA->Surface());
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
        myEvaluator = ExtremaPS_OtherSurface(aGeomAdaptor->Surface());
      }
      break;
    }
  }
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::Perform(const gp_Pnt& theP, double theTol) const
{
  return Perform(theP, myDomain, theTol);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::Perform(const gp_Pnt&              theP,
                                              const ExtremaPS::Domain2D& theDomain,
                                              double                     theTol) const
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
        return theEval.Perform(theP, theDomain, theTol, mySearchMode);
      }
    },
    myEvaluator);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::PerformWithBoundary(const gp_Pnt& theP, double theTol) const
{
  return PerformWithBoundary(theP, myDomain, theTol);
}

//==================================================================================================

ExtremaPS::Result ExtremaPS_Surface::PerformWithBoundary(const gp_Pnt&              theP,
                                                          const ExtremaPS::Domain2D& theDomain,
                                                          double                     theTol) const
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
        return theEval.PerformWithBoundary(theP, theDomain, theTol, mySearchMode);
      }
    },
    myEvaluator);
}

//==================================================================================================

bool ExtremaPS_Surface::IsInitialized() const
{
  return !std::holds_alternative<std::monostate>(myEvaluator);
}
