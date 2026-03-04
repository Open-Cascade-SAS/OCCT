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

#ifndef GeomBndLib_Surface_HeaderFile
#define GeomBndLib_Surface_HeaderFile

#include <GeomBndLib_BezierSurface.hxx>
#include <GeomBndLib_BSplineSurface.hxx>
#include <GeomBndLib_Cone.hxx>
#include <GeomBndLib_Cylinder.hxx>
#include <GeomBndLib_OffsetSurface.hxx>
#include <GeomBndLib_OtherSurface.hxx>
#include <GeomBndLib_Plane.hxx>
#include <GeomBndLib_Sphere.hxx>
#include <GeomBndLib_SurfaceOfExtrusion.hxx>
#include <GeomBndLib_SurfaceOfRevolution.hxx>
#include <GeomBndLib_Torus.hxx>
#include <Bnd_Box.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <variant>

class Adaptor3d_Surface;
class Geom_Surface;

//! Variant-based dispatcher for 3D surface bounding box computation.
//! Auto-detects the surface type and delegates to the appropriate specialized class.
class GeomBndLib_Surface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Construct from an adaptor surface.
  Standard_EXPORT GeomBndLib_Surface(const Adaptor3d_Surface& theSurf);

  //! Construct from a Geom_Surface handle.
  Standard_EXPORT GeomBndLib_Surface(const occ::handle<Geom_Surface>& theSurf);

  GeomBndLib_Surface(const GeomBndLib_Surface&)            = delete;
  GeomBndLib_Surface& operator=(const GeomBndLib_Surface&) = delete;
  GeomBndLib_Surface(GeomBndLib_Surface&&)                 = delete;
  GeomBndLib_Surface& operator=(GeomBndLib_Surface&&)      = delete;

  //! Return detected surface type.
  GeomAbs_SurfaceType GetType() const { return mySurfType; }

  //! Compute bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double   theUMin,
                                            double   theUMax,
                                            double   theVMin,
                                            double   theVMax,
                                            double   theTol) const;

  //! Compute precise bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theTol) const;

  //! Compute precise bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double   theUMin,
                                                   double   theUMax,
                                                   double   theVMin,
                                                   double   theVMax,
                                                   double   theTol) const;

  //! Add bounding box for full surface.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  Standard_EXPORT void Add(double   theUMin,
                           double   theUMax,
                           double   theVMin,
                           double   theVMax,
                           double   theTol,
                           Bnd_Box& theBox) const;

  //! Add precise bounding box for full surface.
  Standard_EXPORT void AddOptimal(double theTol, Bnd_Box& theBox) const;

  //! Add precise bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  Standard_EXPORT void AddOptimal(double   theUMin,
                                  double   theUMax,
                                  double   theVMin,
                                  double   theVMax,
                                  double   theTol,
                                  Bnd_Box& theBox) const;

private:
  using EvaluatorVariant = std::variant<std::monostate,
                                        GeomBndLib_Plane,
                                        GeomBndLib_Cylinder,
                                        GeomBndLib_Cone,
                                        GeomBndLib_Sphere,
                                        GeomBndLib_Torus,
                                        GeomBndLib_BezierSurface,
                                        GeomBndLib_BSplineSurface,
                                        GeomBndLib_SurfaceOfRevolution,
                                        GeomBndLib_SurfaceOfExtrusion,
                                        GeomBndLib_OffsetSurface,
                                        GeomBndLib_OtherSurface>;
  const Adaptor3d_Surface* adaptor() const;
  double effectiveU1() const;
  double effectiveU2() const;
  double effectiveV1() const;
  double effectiveV2() const;

private:
  const Adaptor3d_Surface* myAdaptorRef = nullptr;
  occ::handle<Adaptor3d_Surface> myAdaptorOwned;
  EvaluatorVariant    myEvaluator;
  GeomAbs_SurfaceType mySurfType;
};

#endif // GeomBndLib_Surface_HeaderFile
