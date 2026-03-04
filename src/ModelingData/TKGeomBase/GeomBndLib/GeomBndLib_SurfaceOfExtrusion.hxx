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

#ifndef GeomBndLib_SurfaceOfExtrusion_HeaderFile
#define GeomBndLib_SurfaceOfExtrusion_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a surface of linear extrusion (Geom_SurfaceOfLinearExtrusion).
//! Uses pure analytical approach: P(U, V) = BasisCurve(U) + V * Direction,
//! so the box is computed from the basis curve box extended along the direction.
class GeomBndLib_SurfaceOfExtrusion
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_SurfaceOfExtrusion(const occ::handle<Geom_SurfaceOfLinearExtrusion>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_SurfaceOfExtrusion(const GeomBndLib_SurfaceOfExtrusion&)            = delete;
  GeomBndLib_SurfaceOfExtrusion& operator=(const GeomBndLib_SurfaceOfExtrusion&) = delete;
  GeomBndLib_SurfaceOfExtrusion(GeomBndLib_SurfaceOfExtrusion&&)                 = delete;
  GeomBndLib_SurfaceOfExtrusion& operator=(GeomBndLib_SurfaceOfExtrusion&&)      = delete;

  const occ::handle<Geom_SurfaceOfLinearExtrusion>& Geometry() const { return myGeom; }

  //! Compute bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const;

  //! For this surface type, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box BoxOptimal(double theUMin,
                                   double theUMax,
                                   double theVMin,
                                   double theVMax,
                                   double theTol) const
  {
    return Box(theUMin, theUMax, theVMin, theVMax, theTol);
  }

  //! Compute optimal bounding box for full surface.
  [[nodiscard]] Bnd_Box BoxOptimal(double theTol) const { return Box(theTol); }

private:
  occ::handle<Geom_SurfaceOfLinearExtrusion> myGeom;
};

#endif // GeomBndLib_SurfaceOfExtrusion_HeaderFile
