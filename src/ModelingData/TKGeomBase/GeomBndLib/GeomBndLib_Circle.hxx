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

#ifndef GeomBndLib_Circle_HeaderFile
#define GeomBndLib_Circle_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_Circle.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Circ.hxx>

//! Computes bounding box for a 3D circle (Geom_Circle).
//! Uses analytical per-coordinate extrema computation.
//!
//! Static methods accepting gp_Circ can be used directly without
//! constructing a Geom_Circle handle.
class GeomBndLib_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Circle(const occ::handle<Geom_Circle>& theCircle)
      : myGeom(theCircle)
  {
  }

  GeomBndLib_Circle(const GeomBndLib_Circle&)            = delete;
  GeomBndLib_Circle& operator=(const GeomBndLib_Circle&) = delete;
  GeomBndLib_Circle(GeomBndLib_Circle&&)                 = delete;
  GeomBndLib_Circle& operator=(GeomBndLib_Circle&&)      = delete;

  const occ::handle<Geom_Circle>& Geometry() const { return myGeom; }

  //! Compute bounding box for full circle.
  [[nodiscard]] Bnd_Box Box(double theTol) const { return Box(myGeom->Circ(), theTol); }

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Bnd_Box Box(double theU1, double theU2, double theTol) const
  {
    return Box(myGeom->Circ(), theU1, theU2, theTol);
  }

  //! For analytical curves, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box BoxOptimal(double theU1, double theU2, double theTol) const
  {
    return Box(theU1, theU2, theTol);
  }

  //! Compute bounding box for a full circle defined by gp_Circ.
  [[nodiscard]] Standard_EXPORT static Bnd_Box Box(const gp_Circ& theCirc, double theTol);

  //! Compute bounding box for a circle arc [theU1, theU2] defined by gp_Circ.
  [[nodiscard]] Standard_EXPORT static Bnd_Box Box(const gp_Circ& theCirc,
                                                   double         theU1,
                                                   double         theU2,
                                                   double         theTol);

private:
  occ::handle<Geom_Circle> myGeom;
};

#endif // GeomBndLib_Circle_HeaderFile
