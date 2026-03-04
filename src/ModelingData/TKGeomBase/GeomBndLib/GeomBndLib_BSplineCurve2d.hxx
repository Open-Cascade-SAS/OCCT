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

#ifndef GeomBndLib_BSplineCurve2d_HeaderFile
#define GeomBndLib_BSplineCurve2d_HeaderFile

#include <Bnd_Box2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a 2D BSpline curve (Geom2d_BSplineCurve).
//! Uses poles convex hull with knot-based index selection + sampling.
class GeomBndLib_BSplineCurve2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_BSplineCurve2d(const occ::handle<Geom2d_BSplineCurve>& theCurve)
      : myGeom(theCurve)
  {
  }

  GeomBndLib_BSplineCurve2d(const GeomBndLib_BSplineCurve2d&)            = delete;
  GeomBndLib_BSplineCurve2d& operator=(const GeomBndLib_BSplineCurve2d&) = delete;
  GeomBndLib_BSplineCurve2d(GeomBndLib_BSplineCurve2d&&)                 = delete;
  GeomBndLib_BSplineCurve2d& operator=(GeomBndLib_BSplineCurve2d&&)      = delete;

  const occ::handle<Geom2d_BSplineCurve>& Geometry() const { return myGeom; }

  //! Compute bounding box for full curve.
  [[nodiscard]] Standard_EXPORT Bnd_Box2d Box(double theTol) const;

  //! Compute bounding box for arc [theU1, theU2].
  [[nodiscard]] Standard_EXPORT Bnd_Box2d Box(double theU1, double theU2, double theTol) const;

  //! Compute precise bounding box using numerical optimization.
  [[nodiscard]] Standard_EXPORT Bnd_Box2d BoxOptimal(double theU1, double theU2, double theTol) const;

private:
  occ::handle<Geom2d_BSplineCurve> myGeom;
};

#endif // GeomBndLib_BSplineCurve2d_HeaderFile
