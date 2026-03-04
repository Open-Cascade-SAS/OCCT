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

#ifndef GeomBndLib_OffsetSurface_HeaderFile
#define GeomBndLib_OffsetSurface_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for an offset surface (Geom_OffsetSurface).
//! Computes the bounding box of the basis surface and enlarges it by |offset|.
class GeomBndLib_OffsetSurface
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_OffsetSurface(const occ::handle<Geom_OffsetSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_OffsetSurface(const GeomBndLib_OffsetSurface&)            = delete;
  GeomBndLib_OffsetSurface& operator=(const GeomBndLib_OffsetSurface&) = delete;
  GeomBndLib_OffsetSurface(GeomBndLib_OffsetSurface&&)                 = delete;
  GeomBndLib_OffsetSurface& operator=(GeomBndLib_OffsetSurface&&)      = delete;

  const occ::handle<Geom_OffsetSurface>& Geometry() const { return myGeom; }

  //! Compute bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const;

  //! Compute precise bounding box for full surface.
  [[nodiscard]] Bnd_Box BoxOptimal(double theTol) const { return Box(theTol); }

  //! Compute precise bounding box for surface patch.
  [[nodiscard]] Bnd_Box BoxOptimal(double theUMin,
                                   double theUMax,
                                   double theVMin,
                                   double theVMax,
                                   double theTol) const
  {
    return Box(theUMin, theUMax, theVMin, theVMax, theTol);
  }

private:
  occ::handle<Geom_OffsetSurface> myGeom;
};

#endif // GeomBndLib_OffsetSurface_HeaderFile
