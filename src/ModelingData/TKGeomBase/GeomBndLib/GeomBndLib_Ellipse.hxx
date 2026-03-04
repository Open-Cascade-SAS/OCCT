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

  //! Add bounding box for full ellipse.
  void Add(double theTol, Bnd_Box& theBox) const { Add(myGeom->Elips(), theTol, theBox); }

  //! Add bounding box for arc [theU1, theU2].
  void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(myGeom->Elips(), theU1, theU2, theTol, theBox);
  }

  //! For analytical curves, AddOptimal is same as Add.
  void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
  {
    Add(theU1, theU2, theTol, theBox);
  }

  //! Add bounding box for a full ellipse defined by gp_Elips.
  Standard_EXPORT static void Add(const gp_Elips& theElips, double theTol, Bnd_Box& theBox);

  //! Add bounding box for an ellipse arc [theU1, theU2] defined by gp_Elips.
  Standard_EXPORT static void Add(const gp_Elips& theElips,
                                  double          theU1,
                                  double          theU2,
                                  double          theTol,
                                  Bnd_Box&        theBox);

private:
  occ::handle<Geom_Ellipse> myGeom;
};

#endif // GeomBndLib_Ellipse_HeaderFile
