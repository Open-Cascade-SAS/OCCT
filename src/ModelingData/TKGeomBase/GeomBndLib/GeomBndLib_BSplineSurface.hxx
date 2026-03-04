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

  //! Add precise bounding box using numerical optimization.
  Standard_EXPORT void AddOptimal(double   theUMin,
                                  double   theUMax,
                                  double   theVMin,
                                  double   theVMax,
                                  double   theTol,
                                  Bnd_Box& theBox) const;

private:
  occ::handle<Geom_BSplineSurface> myGeom;
};

#endif // GeomBndLib_BSplineSurface_HeaderFile
