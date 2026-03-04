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

#ifndef GeomBndLib_BezierCurve_HeaderFile
#define GeomBndLib_BezierCurve_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_BezierCurve.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a 3D Bezier curve (Geom_BezierCurve).
//! Uses poles convex hull + sampling for deflection estimation.
class GeomBndLib_BezierCurve
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_BezierCurve(const occ::handle<Geom_BezierCurve>& theCurve)
      : myGeom(theCurve)
  {
  }

  GeomBndLib_BezierCurve(const GeomBndLib_BezierCurve&)            = delete;
  GeomBndLib_BezierCurve& operator=(const GeomBndLib_BezierCurve&) = delete;
  GeomBndLib_BezierCurve(GeomBndLib_BezierCurve&&)                 = delete;
  GeomBndLib_BezierCurve& operator=(GeomBndLib_BezierCurve&&)      = delete;

  const occ::handle<Geom_BezierCurve>& Geometry() const { return myGeom; }

  //! Add bounding box for full curve.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for arc [theU1, theU2].
  Standard_EXPORT void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

  //! Add precise bounding box using numerical optimization.
  Standard_EXPORT void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

private:
  occ::handle<Geom_BezierCurve> myGeom;
};

#endif // GeomBndLib_BezierCurve_HeaderFile
