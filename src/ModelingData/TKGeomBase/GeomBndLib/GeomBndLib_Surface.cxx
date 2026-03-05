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
//! Applies theTrsf to theBox and returns the result.
//! - Translation: exact (shifts bounds, preserves open flags).
//! - Finite box + general transform: 8-corner approximation (valid AABB, may be slightly larger).
//! - Open box + non-translation: returns whole box (safe upper bound).
Bnd_Box transformedBox(const Bnd_Box& theBox, const gp_Trsf& theTrsf)
{
  if (theBox.IsVoid() || theBox.IsWhole())
    return theBox;

  double xMin, yMin, zMin, xMax, yMax, zMax;
  theBox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

  if (theTrsf.Form() == gp_Translation)
  {
    // Exact for pure translations: shift bounds and preserve open flags.
    const gp_XYZ aVec = theTrsf.TranslationPart();
    Bnd_Box      aResult;
    aResult.Update(xMin + aVec.X(),
                   yMin + aVec.Y(),
                   zMin + aVec.Z(),
                   xMax + aVec.X(),
                   yMax + aVec.Y(),
                   zMax + aVec.Z());
    if (theBox.IsOpenXmin())
      aResult.OpenXmin();
    if (theBox.IsOpenXmax())
      aResult.OpenXmax();
    if (theBox.IsOpenYmin())
      aResult.OpenYmin();
    if (theBox.IsOpenYmax())
      aResult.OpenYmax();
    if (theBox.IsOpenZmin())
      aResult.OpenZmin();
    if (theBox.IsOpenZmax())
      aResult.OpenZmax();
    return aResult;
  }

  // For general transforms with open directions: cannot represent correctly as AABB.
  const bool isOpen = theBox.IsOpenXmin() || theBox.IsOpenXmax() || theBox.IsOpenYmin()
                      || theBox.IsOpenYmax() || theBox.IsOpenZmin() || theBox.IsOpenZmax();
  if (isOpen)
  {
    Bnd_Box aResult;
    aResult.SetWhole();
    return aResult;
  }

  // Finite box: transform all 8 corners and build a new AABB.
  Bnd_Box aResult;
  for (int i = 0; i < 8; ++i)
  {
    gp_Pnt aCorner((i & 1) ? xMax : xMin, (i & 2) ? yMax : yMin, (i & 4) ? zMax : zMin);
    aCorner.Transform(theTrsf);
    aResult.Add(aCorner);
  }
  return aResult;
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
    // Use the inner adaptor so all evaluations / UV bounds are in local coordinates.
    // myAdaptorRef now points to the inner GeomAdaptor_Surface (valid as long as theSurf lives).
    myAdaptorRef         = &aGTS->Surface();
    aGeomSurf            = aGTS->GeomSurface(); // untransformed Geom handle
    const gp_Trsf& aTrsf = aGTS->Trsf();
    if (aTrsf.Form() != gp_Identity)
      myTrsf = aTrsf; // deferred to Box/BoxOptimal output
  }

  // Unwrap a trimmed surface so that down_casts below match the surface type
  // that the adaptor already resolved via GetType().
  occ::handle<Geom_Surface> aBaseGeom = aGeomSurf;
  if (!aBaseGeom.IsNull())
    if (const auto aTrim = occ::down_cast<Geom_RectangularTrimmedSurface>(aBaseGeom))
      aBaseGeom = aTrim->BasisSurface();

  switch (mySurfType)
  {
    case GeomAbs_Plane: {
      if (auto aPlane = occ::down_cast<Geom_Plane>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_Plane>(aPlane);
        break;
      }
      myEvaluator.emplace<GeomBndLib_Plane>(new Geom_Plane(myAdaptorRef->Plane()));
      break;
    }
    case GeomAbs_Cylinder: {
      if (auto aCyl = occ::down_cast<Geom_CylindricalSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_Cylinder>(aCyl);
        break;
      }
      myEvaluator.emplace<GeomBndLib_Cylinder>(
        new Geom_CylindricalSurface(myAdaptorRef->Cylinder()));
      break;
    }
    case GeomAbs_Cone: {
      if (auto aCone = occ::down_cast<Geom_ConicalSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_Cone>(aCone);
        break;
      }
      myEvaluator.emplace<GeomBndLib_Cone>(new Geom_ConicalSurface(myAdaptorRef->Cone()));
      break;
    }
    case GeomAbs_Sphere: {
      if (auto aSphere = occ::down_cast<Geom_SphericalSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_Sphere>(aSphere);
        break;
      }
      myEvaluator.emplace<GeomBndLib_Sphere>(new Geom_SphericalSurface(myAdaptorRef->Sphere()));
      break;
    }
    case GeomAbs_Torus: {
      if (auto aTorus = occ::down_cast<Geom_ToroidalSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_Torus>(aTorus);
        break;
      }
      myEvaluator.emplace<GeomBndLib_Torus>(new Geom_ToroidalSurface(myAdaptorRef->Torus()));
      break;
    }
    case GeomAbs_BezierSurface: {
      if (auto aBez = occ::down_cast<Geom_BezierSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_BezierSurface>(aBez);
        break;
      }
      myEvaluator.emplace<GeomBndLib_BezierSurface>(myAdaptorRef->Bezier());
      break;
    }
    case GeomAbs_BSplineSurface: {
      if (auto aBSpl = occ::down_cast<Geom_BSplineSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_BSplineSurface>(aBSpl);
        break;
      }
      myEvaluator.emplace<GeomBndLib_BSplineSurface>(myAdaptorRef->BSpline());
      break;
    }
    case GeomAbs_SurfaceOfRevolution: {
      if (auto aRev = occ::down_cast<Geom_SurfaceOfRevolution>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_SurfaceOfRevolution>(aRev);
        break;
      }
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      if (auto anExtr = occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_SurfaceOfExtrusion>(anExtr);
        break;
      }
      mySurfType = GeomAbs_OtherSurface;
      myEvaluator.emplace<GeomBndLib_OtherSurface>(*myAdaptorRef);
      break;
    }
    case GeomAbs_OffsetSurface: {
      if (auto anOff = occ::down_cast<Geom_OffsetSurface>(aBaseGeom))
      {
        myEvaluator.emplace<GeomBndLib_OffsetSurface>(anOff);
        break;
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
    myAdaptorOwned                          = new GeomAdaptor_Surface(theSurf);
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
    mySurfType     = GeomAbs_OtherSurface;
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

Bnd_Box GeomBndLib_Surface::Box(double theTol) const
{
  if (adaptor() != nullptr)
  {
    return Box(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol);
  }

  Bnd_Box aBox = std::visit(
    [theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        return theEval.Box(theTol);
      }
      return Bnd_Box{};
    },
    myEvaluator);
  if (myTrsf.has_value())
    aBox = transformedBox(aBox, *myTrsf);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::Box(double theUMin,
                                double theUMax,
                                double theVMin,
                                double theVMax,
                                double theTol) const
{
  Bnd_Box aBox = std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        return theEval.Box(theUMin, theUMax, theVMin, theVMax, theTol);
      }
      return Bnd_Box{};
    },
    myEvaluator);
  if (myTrsf.has_value())
    aBox = transformedBox(aBox, *myTrsf);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::BoxOptimal(double theTol) const
{
  if (adaptor() != nullptr)
  {
    return BoxOptimal(effectiveU1(), effectiveU2(), effectiveV1(), effectiveV2(), theTol);
  }

  Bnd_Box aBox = std::visit(
    [theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        return theEval.BoxOptimal(theTol);
      }
      return Bnd_Box{};
    },
    myEvaluator);
  if (myTrsf.has_value())
    aBox = transformedBox(aBox, *myTrsf);
  return aBox;
}

//=================================================================================================

Bnd_Box GeomBndLib_Surface::BoxOptimal(double theUMin,
                                       double theUMax,
                                       double theVMin,
                                       double theVMax,
                                       double theTol) const
{
  Bnd_Box aBox = std::visit(
    [theUMin, theUMax, theVMin, theVMax, theTol](const auto& theEval) -> Bnd_Box {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        return theEval.BoxOptimal(theUMin, theUMax, theVMin, theVMax, theTol);
      }
      return Bnd_Box{};
    },
    myEvaluator);
  if (myTrsf.has_value())
    aBox = transformedBox(aBox, *myTrsf);
  return aBox;
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
