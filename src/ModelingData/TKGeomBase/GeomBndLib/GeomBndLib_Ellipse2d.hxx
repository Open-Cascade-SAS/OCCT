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

#ifndef GeomBndLib_Ellipse2d_HeaderFile
#define GeomBndLib_Ellipse2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Elips2d.hxx>

//! Computes bounding box for a 2D ellipse (Geom2d_Ellipse).
//! Uses analytical per-coordinate extrema computation.
//!
//! Static methods accepting gp_Elips2d can be used directly without
//! constructing a Geom2d_Ellipse handle.
class GeomBndLib_Ellipse2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Ellipse2d(const occ::handle<Geom2d_Ellipse>& theEllipse)
      : myGeom(theEllipse)
  {
  }

  GeomBndLib_Ellipse2d(const GeomBndLib_Ellipse2d&)            = delete;
  GeomBndLib_Ellipse2d& operator=(const GeomBndLib_Ellipse2d&) = delete;
  GeomBndLib_Ellipse2d(GeomBndLib_Ellipse2d&&)                 = delete;
  GeomBndLib_Ellipse2d& operator=(GeomBndLib_Ellipse2d&&)      = delete;

  const occ::handle<Geom2d_Ellipse>& Geometry() const { return myGeom; }

  //! Compute bounding box for full ellipse.
  [[nodiscard]] Bnd_Box2d Box(double theTol) const { return Box(myGeom->Elips2d(), theTol); }

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Bnd_Box2d Box(double theU1, double theU2, double theTol) const
  {
    return Box(myGeom->Elips2d(), theU1, theU2, theTol);
  }

  //! For analytical curves, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box2d BoxOptimal(double theU1, double theU2, double theTol) const
  {
    return Box(theU1, theU2, theTol);
  }

  //! Compute bounding box for a full ellipse defined by gp_Elips2d.
  [[nodiscard]] Standard_EXPORT static Bnd_Box2d Box(const gp_Elips2d& theElips, double theTol);

  //! Compute bounding box for an ellipse arc [theU1, theU2] defined by gp_Elips2d.
  [[nodiscard]] Standard_EXPORT static Bnd_Box2d Box(const gp_Elips2d& theElips,
                                                     double            theU1,
                                                     double            theU2,
                                                     double            theTol);

private:
  occ::handle<Geom2d_Ellipse> myGeom;
};

#endif // GeomBndLib_Ellipse2d_HeaderFile
