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

#include <GeomBndLib_Surface.hxx>

#include <Adaptor3d_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>

#include <type_traits>

//=================================================================================================

GeomBndLib_Surface::GeomBndLib_Surface(const Adaptor3d_Surface& theSurf)
    : mySurfType(theSurf.GetType())
{
  myAdaptorRef = &theSurf;

  switch (mySurfType)
  {
    case GeomAbs_Plane: {
      occ::handle<Geom_Plane> aPlane = new Geom_Plane(theSurf.Plane());
      myEvaluator.emplace<GeomBndLib_Plane>(aPlane);
      break;
    }
    case GeomAbs_Cylinder: {
      occ::handle<Geom_CylindricalSurface> aCyl =
        new Geom_CylindricalSurface(theSurf.Cylinder());
      myEvaluator.emplace<GeomBndLib_Cylinder>(aCyl);
      break;
    }
    case GeomAbs_Cone: {
      occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(theSurf.Cone());
      myEvaluator.emplace<GeomBndLib_Cone>(aCone);
      break;
    }
    case GeomAbs_Sphere: {
      occ::handle<Geom_SphericalSurface> aSphere =
        new Geom_SphericalSurface(theSurf.Sphere());
      myEvaluator.emplace<GeomBndLib_Sphere>(aSphere);
      break;
    }
    case GeomAbs_Torus: {
      occ::handle<Geom_ToroidalSurface> aTorus =
        new Geom_ToroidalSurface(theSurf.Torus());
      myEvaluator.emplace<GeomBndLib_Torus>(aTorus);
      break;
    }
    case GeomAbs_BezierSurface: {
      myEvaluator.emplace<GeomBndLib_BezierSurface>(theSurf.Bezier());
      break;
    }
    case GeomAbs_BSplineSurface: {
      myEvaluator.emplace<GeomBndLib_BSplineSurface>(theSurf.BSpline());
      break;
    }
    case GeomAbs_SurfaceOfRevolution: {
      // Try to get the underlying Geom_SurfaceOfRevolution from the adaptor.
      const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(myAdaptorRef);
      if (aGA != nullptr)
      {
        auto aRev = occ::down_cast<Geom_SurfaceOfRevolution>(aGA->Surface());
        if (!aRev.IsNull())
        {
          myEvaluator.emplace<GeomBndLib_SurfaceOfRevolution>(aRev);
          break;
        }
      }
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(myAdaptorRef);
      if (aGA != nullptr)
      {
        auto anExtr = occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aGA->Surface());
        if (!anExtr.IsNull())
        {
          myEvaluator.emplace<GeomBndLib_SurfaceOfExtrusion>(anExtr);
          break;
        }
      }
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
    case GeomAbs_OffsetSurface: {
      const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(myAdaptorRef);
      if (aGA != nullptr)
      {
        auto anOff = occ::down_cast<Geom_OffsetSurface>(aGA->Surface());
        if (!anOff.IsNull())
        {
          myEvaluator.emplace<GeomBndLib_OffsetSurface>(anOff);
          break;
        }
      }
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
    default: {
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
  }
}

//=================================================================================================

GeomBndLib_Surface::GeomBndLib_Surface(const occ::handle<Geom_Surface>& theSurf)
    : mySurfType(GeomAbs_OtherSurface)
{
  if (auto aTrim = occ::down_cast<Geom_RectangularTrimmedSurface>(theSurf))
  {
    myAdaptorOwned = new GeomAdaptor_Surface(theSurf);
    const occ::handle<Geom_Surface>& aBasis = aTrim->BasisSurface();
    if (auto aPlane = occ::down_cast<Geom_Plane>(aBasis))
    {
      mySurfType = GeomAbs_Plane;
      myEvaluator.emplace<GeomBndLib_Plane>(aPlane);
    }
    else if (auto aCyl = occ::down_cast<Geom_CylindricalSurface>(aBasis))
    {
      mySurfType = GeomAbs_Cylinder;
      myEvaluator.emplace<GeomBndLib_Cylinder>(aCyl);
    }
    else if (auto aCone = occ::down_cast<Geom_ConicalSurface>(aBasis))
    {
      mySurfType = GeomAbs_Cone;
      myEvaluator.emplace<GeomBndLib_Cone>(aCone);
    }
    else if (auto aSphere = occ::down_cast<Geom_SphericalSurface>(aBasis))
    {
      mySurfType = GeomAbs_Sphere;
      myEvaluator.emplace<GeomBndLib_Sphere>(aSphere);
    }
    else if (auto aTorus = occ::down_cast<Geom_ToroidalSurface>(aBasis))
    {
      mySurfType = GeomAbs_Torus;
      myEvaluator.emplace<GeomBndLib_Torus>(aTorus);
    }
    else if (auto aBez = occ::down_cast<Geom_BezierSurface>(aBasis))
    {
      mySurfType = GeomAbs_BezierSurface;
      myEvaluator.emplace<GeomBndLib_BezierSurface>(aBez);
    }
    else if (auto aBSpl = occ::down_cast<Geom_BSplineSurface>(aBasis))
    {
      mySurfType = GeomAbs_BSplineSurface;
      myEvaluator.emplace<GeomBndLib_BSplineSurface>(aBSpl);
    }
    else if (auto aRev = occ::down_cast<Geom_SurfaceOfRevolution>(aBasis))
    {
      mySurfType = GeomAbs_SurfaceOfRevolution;
      myEvaluator.emplace<GeomBndLib_SurfaceOfRevolution>(aRev);
    }
    else if (auto anExtr = occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aBasis))
    {
      mySurfType = GeomAbs_SurfaceOfExtrusion;
      myEvaluator.emplace<GeomBndLib_SurfaceOfExtrusion>(anExtr);
    }
    else if (auto anOff = occ::down_cast<Geom_OffsetSurface>(aBasis))
    {
      mySurfType = GeomAbs_OffsetSurface;
      myEvaluator.emplace<GeomBndLib_OffsetSurface>(anOff);
    }
    else
    {
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorOwned);
    }
    return;
  }

  if (auto aPlane = occ::down_cast<Geom_Plane>(theSurf))
  {
    mySurfType = GeomAbs_Plane;
    myEvaluator.emplace<GeomBndLib_Plane>(aPlane);
  }
  else if (auto aCyl = occ::down_cast<Geom_CylindricalSurface>(theSurf))
  {
    mySurfType = GeomAbs_Cylinder;
    myEvaluator.emplace<GeomBndLib_Cylinder>(aCyl);
  }
  else if (auto aCone = occ::down_cast<Geom_ConicalSurface>(theSurf))
  {
    mySurfType = GeomAbs_Cone;
    myEvaluator.emplace<GeomBndLib_Cone>(aCone);
  }
  else if (auto aSphere = occ::down_cast<Geom_SphericalSurface>(theSurf))
  {
    mySurfType = GeomAbs_Sphere;
    myEvaluator.emplace<GeomBndLib_Sphere>(aSphere);
  }
  else if (auto aTorus = occ::down_cast<Geom_ToroidalSurface>(theSurf))
  {
    mySurfType = GeomAbs_Torus;
    myEvaluator.emplace<GeomBndLib_Torus>(aTorus);
  }
  else if (auto aBez = occ::down_cast<Geom_BezierSurface>(theSurf))
  {
    mySurfType = GeomAbs_BezierSurface;
    myEvaluator.emplace<GeomBndLib_BezierSurface>(aBez);
  }
  else if (auto aBSpl = occ::down_cast<Geom_BSplineSurface>(theSurf))
  {
    mySurfType = GeomAbs_BSplineSurface;
    myEvaluator.emplace<GeomBndLib_BSplineSurface>(aBSpl);
  }
  else if (auto aRev = occ::down_cast<Geom_SurfaceOfRevolution>(theSurf))
  {
    mySurfType = GeomAbs_SurfaceOfRevolution;
    myEvaluator.emplace<GeomBndLib_SurfaceOfRevolution>(aRev);
  }
  else if (auto anExtr = occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurf))
  {
    mySurfType = GeomAbs_SurfaceOfExtrusion;
    myEvaluator.emplace<GeomBndLib_SurfaceOfExtrusion>(anExtr);
  }
  else if (auto anOff = occ::down_cast<Geom_OffsetSurface>(theSurf))
  {
    mySurfType = GeomAbs_OffsetSurface;
    myEvaluator.emplace<GeomBndLib_OffsetSurface>(anOff);
  }
  else
  {
    myAdaptorOwned = new GeomAdaptor_Surface(theSurf);
    mySurfType = GeomAbs_OtherSurface;
    myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorOwned);
  }
}

//=================================================================================================

const Adaptor3d_Surface* GeomBndLib_Surface::adaptor() const
{
  return myAdaptorRef != nullptr ? myAdaptorRef
                                 : (myAdaptorOwned.IsNull() ? nullptr : &*myAdaptorOwned);
}

//=================================================================================================

double GeomBndLib_Surface::effectiveU1() const
{
  const Adaptor3d_Surface* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->FirstUParameter() : 0.0;
}

//=================================================================================================

double GeomBndLib_Surface::effectiveU2() const
{
  const Adaptor3d_Surface* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->LastUParameter() : 0.0;
}

//=================================================================================================

double GeomBndLib_Surface::effectiveV1() const
{
  const Adaptor3d_Surface* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->FirstVParameter() : 0.0;
}

//=================================================================================================

double GeomBndLib_Surface::effectiveV2() const
{
  const Adaptor3d_Surface* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->LastVParameter() : 0.0;
}

//=================================================================================================

void GeomBndLib_Surface::Add(double theTol, Bnd_Box& theBox) const
{
  if (adaptor() != nullptr)
  {
    Add(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol, theBox);
    return;
  }
  std::visit(
    [theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.Add(theTol, theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Surface::Add(double   theUMin,
                             double   theUMax,
                             double   theVMin,
                             double   theVMax,
                             double   theTol,
                             Bnd_Box& theBox) const
{
  std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.Add(theUMin, theUMax, theVMin, theVMax, theTol, theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Surface::AddOptimal(double theTol, Bnd_Box& theBox) const
{
  if (adaptor() != nullptr)
  {
    AddOptimal(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol, theBox);
    return;
  }
  std::visit(
    [theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.AddOptimal(theTol, theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Surface::AddOptimal(double   theUMin,
                                    double   theUMax,
                                    double   theVMin,
                                    double   theVMax,
                                    double   theTol,
                                    Bnd_Box& theBox) const
{
  std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.AddOptimal(theUMin, theUMax, theVMin, theVMax, theTol, theBox);
      }
    },
    myEvaluator);
}
