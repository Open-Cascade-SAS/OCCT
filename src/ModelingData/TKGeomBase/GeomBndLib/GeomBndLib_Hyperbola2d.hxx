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

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
  {
    Add(myGeom->Hypr2d(), theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for full curve.
  void Add(double theTol, Bnd_Box2d& theBox) const
  {
    Add(myGeom->FirstParameter(), myGeom->LastParameter(), theTol, theBox);
  }

  //! For analytical curves, AddOptimal is same as Add.
  void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
  {
    Add(theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for a 2D hyperbola arc [theU1, theU2] defined by gp_Hypr2d.
  Standard_EXPORT static void Add(const gp_Hypr2d& theHypr,
                                  double           theU1,
                                  double           theU2,
                                  double           theTol,
                                  Bnd_Box2d&       theBox);

private:
  occ::handle<Geom2d_Hyperbola> myGeom;
};

#endif // GeomBndLib_Hyperbola2d_HeaderFile
