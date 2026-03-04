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

#ifndef GeomBndLib_OffsetCurve_HeaderFile
#define GeomBndLib_OffsetCurve_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a 3D offset curve (Geom_OffsetCurve).
//! Computes the bounding box of the basis curve and enlarges it by |offset|.
class GeomBndLib_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_OffsetCurve(const occ::handle<Geom_OffsetCurve>& theCurve)
      : myGeom(theCurve)
  {
  }

  GeomBndLib_OffsetCurve(const GeomBndLib_OffsetCurve&)            = delete;
  GeomBndLib_OffsetCurve& operator=(const GeomBndLib_OffsetCurve&) = delete;
  GeomBndLib_OffsetCurve(GeomBndLib_OffsetCurve&&)                 = delete;
  GeomBndLib_OffsetCurve& operator=(GeomBndLib_OffsetCurve&&)      = delete;

  const occ::handle<Geom_OffsetCurve>& Geometry() const { return myGeom; }

  //! Compute bounding box for full curve.
  [[nodiscard]] Bnd_Box Box(double theTol) const
  {
    return Box(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
  }

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theU1, double theU2, double theTol) const;

  //! Compute precise bounding box for full curve.
  [[nodiscard]] Bnd_Box BoxOptimal(double theTol) const
  {
    return BoxOptimal(myGeom->FirstParameter(), myGeom->LastParameter(), theTol);
  }

  //! Compute precise bounding box for arc [theU1, theU2] by sampling the offset curve.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theU1,
                                                   double theU2,
                                                   double theTol) const;

private:
  occ::handle<Geom_OffsetCurve> myGeom;
};

#endif // GeomBndLib_OffsetCurve_HeaderFile
