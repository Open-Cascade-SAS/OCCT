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

#ifndef GeomBndLib_Ellipse_HeaderFile
#define GeomBndLib_Ellipse_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_Ellipse.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Elips.hxx>

//! Computes bounding box for a 3D ellipse (Geom_Ellipse).
//! Uses analytical per-coordinate extrema computation.
//!
//! Static methods accepting gp_Elips can be used directly without
//! constructing a Geom_Ellipse handle.
class GeomBndLib_Ellipse
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Ellipse(const occ::handle<Geom_Ellipse>& theEllipse)
      : myGeom(theEllipse)
  {
  }

  GeomBndLib_Ellipse(const GeomBndLib_Ellipse&)            = delete;
  GeomBndLib_Ellipse& operator=(const GeomBndLib_Ellipse&) = delete;
  GeomBndLib_Ellipse(GeomBndLib_Ellipse&&)                 = delete;
  GeomBndLib_Ellipse& operator=(GeomBndLib_Ellipse&&)      = delete;

  const occ::handle<Geom_Ellipse>& Geometry() const { return myGeom; }

  //! Compute bounding box for full ellipse.
  [[nodiscard]] Bnd_Box Box(double theTol) const { return Box(myGeom->Elips(), theTol); }

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Bnd_Box Box(double theU1, double theU2, double theTol) const
  {
    return Box(myGeom->Elips(), theU1, theU2, theTol);
  }

  //! For analytical curves, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box BoxOptimal(double theU1, double theU2, double theTol) const
  {
    return Box(theU1, theU2, theTol);
  }

  //! Compute bounding box for a full ellipse defined by gp_Elips.
  [[nodiscard]] Standard_EXPORT static Bnd_Box Box(const gp_Elips& theElips, double theTol);

  //! Compute bounding box for an ellipse arc [theU1, theU2] defined by gp_Elips.
  [[nodiscard]] Standard_EXPORT static Bnd_Box Box(const gp_Elips& theElips,
                                                   double          theU1,
                                                   double          theU2,
                                                   double          theTol);

private:
  occ::handle<Geom_Ellipse> myGeom;
};

#endif // GeomBndLib_Ellipse_HeaderFile
