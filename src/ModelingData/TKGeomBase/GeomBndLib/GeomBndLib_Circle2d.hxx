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

#ifndef GeomBndLib_Circle2d_HeaderFile
#define GeomBndLib_Circle2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Circ2d.hxx>

//! Computes bounding box for a 2D circle (Geom2d_Circle).
//! Uses analytical per-coordinate extrema computation.
//!
//! Static methods accepting gp_Circ2d can be used directly without
//! constructing a Geom2d_Circle handle.
class GeomBndLib_Circle2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Circle2d(const occ::handle<Geom2d_Circle>& theCircle)
      : myGeom(theCircle)
  {
  }

  GeomBndLib_Circle2d(const GeomBndLib_Circle2d&)            = delete;
  GeomBndLib_Circle2d& operator=(const GeomBndLib_Circle2d&) = delete;
  GeomBndLib_Circle2d(GeomBndLib_Circle2d&&)                 = delete;
  GeomBndLib_Circle2d& operator=(GeomBndLib_Circle2d&&)      = delete;

  const occ::handle<Geom2d_Circle>& Geometry() const { return myGeom; }

  //! Add bounding box for full circle.
  void Add(double theTol, Bnd_Box2d& theBox) const { Add(myGeom->Circ2d(), theTol, theBox); }

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
  {
    Add(myGeom->Circ2d(), theU1, theU2, theTol, theBox);
  }

  //! For analytical curves, AddOptimal is same as Add.
  void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
  {
    Add(theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for a full circle defined by gp_Circ2d.
  Standard_EXPORT static void Add(const gp_Circ2d& theCirc, double theTol, Bnd_Box2d& theBox);

  //! Add bounding box for a circle arc [theU1, theU2] defined by gp_Circ2d.
  Standard_EXPORT static void Add(const gp_Circ2d& theCirc,
                                  double           theU1,
                                  double           theU2,
                                  double           theTol,
                                  Bnd_Box2d&       theBox);

private:
  occ::handle<Geom2d_Circle> myGeom;
};

#endif // GeomBndLib_Circle2d_HeaderFile
