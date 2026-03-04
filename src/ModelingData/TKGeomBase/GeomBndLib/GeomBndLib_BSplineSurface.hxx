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

#ifndef GeomBndLib_BSplineSurface_HeaderFile
#define GeomBndLib_BSplineSurface_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a BSpline surface (Geom_BSplineSurface).
//! Uses poles convex hull with knot-based index selection via ComputePolesIndexes.
class GeomBndLib_BSplineSurface
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_BSplineSurface(const occ::handle<Geom_BSplineSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_BSplineSurface(const GeomBndLib_BSplineSurface&)            = delete;
  GeomBndLib_BSplineSurface& operator=(const GeomBndLib_BSplineSurface&) = delete;
  GeomBndLib_BSplineSurface(GeomBndLib_BSplineSurface&&)                 = delete;
  GeomBndLib_BSplineSurface& operator=(GeomBndLib_BSplineSurface&&)      = delete;

  const occ::handle<Geom_BSplineSurface>& Geometry() const { return myGeom; }

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
  occ::handle<Geom_BSplineSurface> myGeom;
};

#endif // GeomBndLib_BSplineSurface_HeaderFile
