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

#ifndef GeomBndLib_Parabola_HeaderFile
#define GeomBndLib_Parabola_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_Parabola.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <gp_Parab.hxx>

//! Computes bounding box for a 3D parabola (Geom_Parabola).
//! Handles infinite parameters by opening the box in appropriate directions.
//!
//! Static method accepting gp_Parab can be used directly without
//! constructing a Geom_Parabola handle.
class GeomBndLib_Parabola
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Parabola(const occ::handle<Geom_Parabola>& theParabola)
      : myGeom(theParabola)
  {
  }

  GeomBndLib_Parabola(const GeomBndLib_Parabola&)            = delete;
  GeomBndLib_Parabola& operator=(const GeomBndLib_Parabola&) = delete;
  GeomBndLib_Parabola(GeomBndLib_Parabola&&)                 = delete;
  GeomBndLib_Parabola& operator=(GeomBndLib_Parabola&&)      = delete;

  const occ::handle<Geom_Parabola>& Geometry() const { return myGeom; }

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->Parab(), theU1, theU2, theTol, theBox);
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

  //! Add bounding box for a parabola arc [theU1, theU2] defined by gp_Parab.
  Standard_EXPORT static void Add(const gp_Parab& theParab,
                                  double          theU1,
                                  double          theU2,
                                  double          theTol,
                                  Bnd_Box&        theBox);

private:
  occ::handle<Geom_Parabola> myGeom;
};

#endif // GeomBndLib_Parabola_HeaderFile
