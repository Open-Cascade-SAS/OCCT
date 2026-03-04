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

#ifndef GeomBndLib_Parabola2d_HeaderFile
#define GeomBndLib_Parabola2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Geom2d_Parabola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Parab2d.hxx>

//! Computes bounding box for a 2D parabola (Geom2d_Parabola).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Parab2d can be used directly without
//! constructing a Geom2d_Parabola handle.
class GeomBndLib_Parabola2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Parabola2d(const occ::handle<Geom2d_Parabola>& theParabola)
      : myGeom(theParabola)
  {
  }

  GeomBndLib_Parabola2d(const GeomBndLib_Parabola2d&)            = delete;
  GeomBndLib_Parabola2d& operator=(const GeomBndLib_Parabola2d&) = delete;
  GeomBndLib_Parabola2d(GeomBndLib_Parabola2d&&)                 = delete;
  GeomBndLib_Parabola2d& operator=(GeomBndLib_Parabola2d&&)      = delete;

  const occ::handle<Geom2d_Parabola>& Geometry() const { return myGeom; }

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
  {
    Add(myGeom->Parab2d(), theU1, theU2, theTol, theBox);
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

  //! Add bounding box for a 2D parabola arc [theU1, theU2] defined by gp_Parab2d.
  Standard_EXPORT static void Add(const gp_Parab2d& theParab,
                                  double            theU1,
                                  double            theU2,
                                  double            theTol,
                                  Bnd_Box2d&        theBox);

private:
  occ::handle<Geom2d_Parabola> myGeom;
};

#endif // GeomBndLib_Parabola2d_HeaderFile
