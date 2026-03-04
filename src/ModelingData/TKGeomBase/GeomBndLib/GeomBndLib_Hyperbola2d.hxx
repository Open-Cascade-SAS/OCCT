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

#ifndef GeomBndLib_Hyperbola2d_HeaderFile
#define GeomBndLib_Hyperbola2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Hypr2d.hxx>

//! Computes bounding box for a 2D hyperbola (Geom2d_Hyperbola).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Hypr2d can be used directly without
//! constructing a Geom2d_Hyperbola handle.
class GeomBndLib_Hyperbola2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Hyperbola2d(const occ::handle<Geom2d_Hyperbola>& theHyperbola)
      : myGeom(theHyperbola)
  {
  }

  GeomBndLib_Hyperbola2d(const GeomBndLib_Hyperbola2d&)            = delete;
  GeomBndLib_Hyperbola2d& operator=(const GeomBndLib_Hyperbola2d&) = delete;
  GeomBndLib_Hyperbola2d(GeomBndLib_Hyperbola2d&&)                 = delete;
  GeomBndLib_Hyperbola2d& operator=(GeomBndLib_Hyperbola2d&&)      = delete;

  const occ::handle<Geom2d_Hyperbola>& Geometry() const { return myGeom; }

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Bnd_Box2d Box(double theU1, double theU2, double theTol) const
  {
    return Box(myGeom->Hypr2d(), theU1, theU2, theTol);
  }

  //! Compute bounding box for full curve.
  [[nodiscard]] Bnd_Box2d Box(double theTol) const
  {
    return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
  }

  //! For analytical curves, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box2d BoxOptimal(double theU1, double theU2, double theTol) const
  {
    return Box(theU1, theU2, theTol);
  }

  //! Compute bounding box for a 2D hyperbola arc [theU1, theU2] defined by gp_Hypr2d.
  [[nodiscard]] Standard_EXPORT static Bnd_Box2d Box(const gp_Hypr2d& theHypr,
                                                     double           theU1,
                                                     double           theU2,
                                                     double           theTol);

private:
  occ::handle<Geom2d_Hyperbola> myGeom;
};

#endif // GeomBndLib_Hyperbola2d_HeaderFile
