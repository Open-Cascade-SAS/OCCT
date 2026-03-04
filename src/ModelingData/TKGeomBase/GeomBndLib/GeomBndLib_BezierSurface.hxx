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

#ifndef GeomBndLib_BezierSurface_HeaderFile
#define GeomBndLib_BezierSurface_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_BezierSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a Bezier surface (Geom_BezierSurface).
//! Uses poles convex hull for full surface, grid sampling for trimmed patches.
class GeomBndLib_BezierSurface
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_BezierSurface(const occ::handle<Geom_BezierSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_BezierSurface(const GeomBndLib_BezierSurface&)            = delete;
  GeomBndLib_BezierSurface& operator=(const GeomBndLib_BezierSurface&) = delete;
  GeomBndLib_BezierSurface(GeomBndLib_BezierSurface&&)                 = delete;
  GeomBndLib_BezierSurface& operator=(GeomBndLib_BezierSurface&&)      = delete;

  const occ::handle<Geom_BezierSurface>& Geometry() const { return myGeom; }

  //! Compute bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const;

  //! Compute precise bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theTol) const;

  //! Compute precise bounding box using numerical optimization.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theUMin,
                                                   double theUMax,
                                                   double theVMin,
                                                   double theVMax,
                                                   double theTol) const;

private:
  occ::handle<Geom_BezierSurface> myGeom;
};

#endif // GeomBndLib_BezierSurface_HeaderFile
