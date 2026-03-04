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
  occ::handle<Geom_BezierSurface> myGeom;
};

#endif // GeomBndLib_BezierSurface_HeaderFile
