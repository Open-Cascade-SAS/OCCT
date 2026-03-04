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

#ifndef GeomBndLib_Hyperbola_HeaderFile
#define GeomBndLib_Hyperbola_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_Hyperbola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Hypr.hxx>

//! Computes bounding box for a 3D hyperbola (Geom_Hyperbola).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Hypr can be used directly without
//! constructing a Geom_Hyperbola handle.
class GeomBndLib_Hyperbola
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Hyperbola(const occ::handle<Geom_Hyperbola>& theHyperbola)
      : myGeom(theHyperbola)
  {
  }

  GeomBndLib_Hyperbola(const GeomBndLib_Hyperbola&)            = delete;
  GeomBndLib_Hyperbola& operator=(const GeomBndLib_Hyperbola&) = delete;
  GeomBndLib_Hyperbola(GeomBndLib_Hyperbola&&)                 = delete;
  GeomBndLib_Hyperbola& operator=(GeomBndLib_Hyperbola&&)      = delete;

  const occ::handle<Geom_Hyperbola>& Geometry() const { return myGeom; }

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->Hypr(), theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for full curve.
  void Add(double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->FirstParameter(), myGeom->LastParameter(), theTol, theBox);
  }

  //! For analytical curves, AddOptimal is same as Add.
  void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for a hyperbola arc [theU1, theU2] defined by gp_Hypr.
  Standard_EXPORT static void Add(const gp_Hypr& theHypr,
                                  double         theU1,
                                  double         theU2,
                                  double         theTol,
                                  Bnd_Box&       theBox);

private:
  occ::handle<Geom_Hyperbola> myGeom;
};

#endif // GeomBndLib_Hyperbola_HeaderFile
