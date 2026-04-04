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
#include <GeomAdaptor_TransformedSurface.hxx>
#include <gp_TrsfForm.hxx>

#include <type_traits>

namespace
{
occ::handle<Geom_Surface> unwrapSurface(const occ::handle<Geom_Surface>& theSurf)
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    occ::down_cast<Geom_RectangularTrimmedSurface>(theSurf);
  return aTrim.IsNull() ? theSurf : unwrapSurface(aTrim->BasisSurface());
}
} // namespace

//=================================================================================================

GeomBndLib_Surface::GeomBndLib_Surface(const Adaptor3d_Surface& theSurf)
    : mySurfType(theSurf.GetType())
{
  myAdaptorRef = &theSurf;

  // Detect the adaptor type ONCE and extract the underlying Geom_Surface handle.
  // For GeomAdaptor_TransformedSurface (which BRepAdaptor_Surface inherits from):
  //   - use the inner GeomAdaptor_Surface as myAdaptorRef (untransformed UV bounds / eval)
  //   - store the transformation in myTrsf for application at the Box/BoxOptimal level
  //   - no Copy+Transform of the geometry - transformation is deferred to box post-processing
  occ::handle<Geom_Surface> aGeomSurf;
  if (const GeomAdaptor_Surface* aGA = dynamic_cast<const GeomAdaptor_Surface*>(&theSurf))
  {
    aGeomSurf = aGA->Surface();
  }
  else if (const GeomAdaptor_TransformedSurface* aGTS =
             dynamic_cast<const GeomAdaptor_TransformedSurface*>(&theSurf))
  {
    aGeomSurf = aGTS->GeomSurfaceTransformed();
  }

  const occ::handle<Geom_Surface> aBaseGeom = unwrapSurface(aGeomSurf);
  if (!aBaseGeom.IsNull() && initFromSurfaceType(mySurfType, aBaseGeom))
  {
    return;
  }

  switch (mySurfType)
  {
    case GeomAbs_Plane: {
      myEvaluator.emplace<GeomBndLib_Plane>(new Geom_Plane(myAdaptorRef->Plane()));
      break;
    }
    case GeomAbs_Cylinder: {
      myEvaluator.emplace<GeomBndLib_Cylinder>(
        new Geom_CylindricalSurface(myAdaptorRef->Cylinder()));
      break;
    }
    case GeomAbs_Cone: {
      myEvaluator.emplace<GeomBndLib_Cone>(new Geom_ConicalSurface(myAdaptorRef->Cone()));
      break;
    }
    case GeomAbs_Sphere: {
      myEvaluator.emplace<GeomBndLib_Sphere>(new Geom_SphericalSurface(myAdaptorRef->Sphere()));
      break;
    }
    case GeomAbs_Torus: {
      myEvaluator.emplace<GeomBndLib_Torus>(new Geom_ToroidalSurface(myAdaptorRef->Torus()));
      break;
    }
    case GeomAbs_BezierSurface: {
      myEvaluator.emplace<GeomBndLib_BezierSurface>(myAdaptorRef->Bezier());
      break;
    }
    case GeomAbs_BSplineSurface: {
      myEvaluator.emplace<GeomBndLib_BSplineSurface>(myAdaptorRef->BSpline());
      break;
    }
    default: {
      initOtherSurface(*myAdaptorRef);
      break;
    }
  }
}

//=================================================================================================

GeomBndLib_Surface::GeomBndLib_Surface(const occ::handle<Geom_Surface>& theSurf)
    : mySurfType(GeomAbs_OtherSurface)
{
  const occ::handle<Geom_RectangularTrimmedSurface> aTrim =
    occ::down_cast<Geom_RectangularTrimmedSurface>(theSurf);
  if (!aTrim.IsNull())
  {
    myAdaptorOwned = new GeomAdaptor_Surface(theSurf);
    if (!initFromSurface(aTrim->BasisSurface()))
    {
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorOwned);
    }
    return;
  }

  if (!initFromSurface(theSurf))
  {
    initOtherSurface(theSurf);
  }
}

//=================================================================================================

bool GeomBndLib_Surface::initFromSurface(const occ::handle<Geom_Surface>& theSurf)
{
  if (theSurf.IsNull())
  {
    return false;
  }

  const occ::handle<Standard_Type> aType = theSurf->DynamicType();
  GeomAbs_SurfaceType              aSurfType;
  if (aType == STANDARD_TYPE(Geom_Plane))
  {
    aSurfType = GeomAbs_Plane;
  }
  else if (aType == STANDARD_TYPE(Geom_CylindricalSurface))
  {
    aSurfType = GeomAbs_Cylinder;
  }
  else if (aType == STANDARD_TYPE(Geom_ConicalSurface))
  {
    aSurfType = GeomAbs_Cone;
  }
  else if (aType == STANDARD_TYPE(Geom_SphericalSurface))
  {
    aSurfType = GeomAbs_Sphere;
  }
  else if (aType == STANDARD_TYPE(Geom_ToroidalSurface))
  {
    aSurfType = GeomAbs_Torus;
  }
  else if (aType == STANDARD_TYPE(Geom_BezierSurface))
  {
    aSurfType = GeomAbs_BezierSurface;
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineSurface))
  {
    aSurfType = GeomAbs_BSplineSurface;
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
  {
    aSurfType = GeomAbs_SurfaceOfRevolution;
  }
  else if (aType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
  {
    aSurfType = GeomAbs_SurfaceOfExtrusion;
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetSurface))
  {
    aSurfType = GeomAbs_OffsetSurface;
  }
  else
  {
    return false;
  }

  mySurfType = aSurfType;
  return initFromSurfaceType(aSurfType, theSurf);
}

//=================================================================================================

bool GeomBndLib_Surface::initFromSurfaceType(GeomAbs_SurfaceType              theType,
                                             const occ::handle<Geom_Surface>& theSurf)
{
  switch (theType)
  {
    case GeomAbs_Plane: {
      const occ::handle<Geom_Plane> aPlane = occ::down_cast<Geom_Plane>(theSurf);
      myEvaluator.emplace<GeomBndLib_Plane>(aPlane);
      return true;
    }
    case GeomAbs_Cylinder: {
      const occ::handle<Geom_CylindricalSurface> aCylinder =
        occ::down_cast<Geom_CylindricalSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_Cylinder>(aCylinder);
      return true;
    }
    case GeomAbs_Cone: {
      const occ::handle<Geom_ConicalSurface> aCone = occ::down_cast<Geom_ConicalSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_Cone>(aCone);
      return true;
    }
    case GeomAbs_Sphere: {
      const occ::handle<Geom_SphericalSurface> aSphere =
        occ::down_cast<Geom_SphericalSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_Sphere>(aSphere);
      return true;
    }
    case GeomAbs_Torus: {
      const occ::handle<Geom_ToroidalSurface> aTorus =
        occ::down_cast<Geom_ToroidalSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_Torus>(aTorus);
      return true;
    }
    case GeomAbs_BezierSurface: {
      const occ::handle<Geom_BezierSurface> aBezier = occ::down_cast<Geom_BezierSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_BezierSurface>(aBezier);
      return true;
    }
    case GeomAbs_BSplineSurface: {
      const occ::handle<Geom_BSplineSurface> aBSpline =
        occ::down_cast<Geom_BSplineSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_BSplineSurface>(aBSpline);
      return true;
    }
    case GeomAbs_SurfaceOfRevolution: {
      const occ::handle<Geom_SurfaceOfRevolution> aRevolution =
        occ::down_cast<Geom_SurfaceOfRevolution>(theSurf);
      myEvaluator.emplace<GeomBndLib_SurfaceOfRevolution>(aRevolution);
      return true;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      const occ::handle<Geom_SurfaceOfLinearExtrusion> anExtrusion =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(theSurf);
      myEvaluator.emplace<GeomBndLib_SurfaceOfExtrusion>(anExtrusion);
      return true;
    }
    case GeomAbs_OffsetSurface: {
      const occ::handle<Geom_OffsetSurface> anOffset = occ::down_cast<Geom_OffsetSurface>(theSurf);
      myEvaluator.emplace<GeomBndLib_OffsetSurface>(anOffset);
      return true;
    }
    default:
      return false;
  }
}

//=================================================================================================

void GeomBndLib_Surface::initOtherSurface(const Adaptor3d_Surface& theSurf)
{
  mySurfType = GeomAbs_OtherSurface;
  myEvaluator.emplace<GeomBndLib_OtherSurface>(theSurf);
}

//=================================================================================================

void GeomBndLib_Surface::initOtherSurface(const occ::handle<Geom_Surface>& theSurf)
{
  myAdaptorOwned = new GeomAdaptor_Surface(theSurf);
  mySurfType     = GeomAbs_OtherSurface;
  myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorOwned);
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

Bnd_Box GeomBndLib_Surface::Box(double theTol) const
{
  if (adaptor() != nullptr)
  {
    return Box(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol);
  }

  return std::visit(
    [theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Bnd_Box{};
      }
      else
      {
        return theEval.Box(theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::Box(double theUMin,
                                double theUMax,
                                double theVMin,
                                double theVMax,
                                double theTol) const
{
  return std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Bnd_Box{};
      }
      else
      {
        return theEval.Box(theUMin, theUMax, theVMin, theVMax, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::BoxOptimal(double theTol) const
{
  if (adaptor() != nullptr)
  {
    return BoxOptimal(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol);
  }

  return std::visit(
    [theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Bnd_Box{};
      }
      else
      {
        return theEval.BoxOptimal(theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::BoxOptimal(double theUMin,
                                       double theUMax,
                                       double theVMin,
                                       double theVMax,
                                       double theTol) const
{
  return std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Bnd_Box{};
      }
      else
      {
        return theEval.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Surface::Add(double theTol, Bnd_Box& theBox) const
{
  theBox.Add(Box(theTol));
}

//=================================================================================================

void GeomBndLib_Surface::Add(double   theUMin,
                             double   theUMax,
                             double   theVMin,
                             double   theVMax,
                             double   theTol,
                             Bnd_Box& theBox) const
{
  theBox.Add(Box(theUMin, theUMax, theVMin, theVMax, theTol));
}

//=================================================================================================

void GeomBndLib_Surface::AddOptimal(double theTol, Bnd_Box& theBox) const
{
  theBox.Add(BoxOptimal(theTol));
}

//=================================================================================================

void GeomBndLib_Surface::AddOptimal(double   theUMin,
                                    double   theUMax,
                                    double   theVMin,
                                    double   theVMax,
                                    double   theTol,
                                    Bnd_Box& theBox) const
{
  theBox.Add(BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol));
}
