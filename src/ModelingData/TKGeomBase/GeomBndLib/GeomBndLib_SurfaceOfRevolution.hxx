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

#ifndef GeomBndLib_SurfaceOfRevolution_HeaderFile
#define GeomBndLib_SurfaceOfRevolution_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a surface of revolution (Geom_SurfaceOfRevolution).
//! Uses analytical approach: samples the basis curve at multiple V values,
//! constructs the revolution circle for each sample point, and bounds each
//! circle arc using GeomBndLib_Circle.
class GeomBndLib_SurfaceOfRevolution
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_SurfaceOfRevolution(const occ::handle<Geom_SurfaceOfRevolution>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_SurfaceOfRevolution(const GeomBndLib_SurfaceOfRevolution&)            = delete;
  GeomBndLib_SurfaceOfRevolution& operator=(const GeomBndLib_SurfaceOfRevolution&) = delete;
  GeomBndLib_SurfaceOfRevolution(GeomBndLib_SurfaceOfRevolution&&)                 = delete;
  GeomBndLib_SurfaceOfRevolution& operator=(GeomBndLib_SurfaceOfRevolution&&)      = delete;

  const occ::handle<Geom_SurfaceOfRevolution>& Geometry() const { return myGeom; }

  //! Compute bounding box for full surface.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const;

  //! Compute precise bounding box using tight basis curve bounds.
  [[nodiscard]] Standard_EXPORT Bnd_Box BoxOptimal(double theUMin,
                                                   double theUMax,
                                                   double theVMin,
                                                   double theVMax,
                                                   double theTol) const;

  //! Compute precise bounding box for full surface.
  [[nodiscard]] Bnd_Box BoxOptimal(double theTol) const
  {
    double aU1 = 0., aU2 = 0., aV1 = 0., aV2 = 0.;
    myGeom->Bounds(aU1, aU2, aV1, aV2);
    return BoxOptimal(aU1, aU2, aV1, aV2, theTol);
  }

private:
  occ::handle<Geom_SurfaceOfRevolution> myGeom;
};

#endif // GeomBndLib_SurfaceOfRevolution_HeaderFile
