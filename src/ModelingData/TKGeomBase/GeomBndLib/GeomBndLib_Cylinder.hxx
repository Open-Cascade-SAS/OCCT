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

#ifndef GeomBndLib_Cylinder_HeaderFile
#define GeomBndLib_Cylinder_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a cylindrical surface (Geom_CylindricalSurface).
//! Uses ElSLib iso-curves and GeomBndLib_ConicHelpers for circle arc bounding.
class GeomBndLib_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Cylinder(const occ::handle<Geom_CylindricalSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_Cylinder(const GeomBndLib_Cylinder&)            = delete;
  GeomBndLib_Cylinder& operator=(const GeomBndLib_Cylinder&) = delete;
  GeomBndLib_Cylinder(GeomBndLib_Cylinder&&)                 = delete;
  GeomBndLib_Cylinder& operator=(GeomBndLib_Cylinder&&)      = delete;

  const occ::handle<Geom_CylindricalSurface>& Geometry() const { return myGeom; }

  //! Compute bounding box for full cylinder.
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theTol) const;

  //! Compute bounding box for cylinder patch [theUMin, theUMax] x [theVMin, theVMax].
  [[nodiscard]] Standard_EXPORT Bnd_Box Box(double theUMin,
                                            double theUMax,
                                            double theVMin,
                                            double theVMax,
                                            double theTol) const;

  //! For analytical surfaces, BoxOptimal is same as Box.
  [[nodiscard]] Bnd_Box BoxOptimal(double theUMin,
                                   double theUMax,
                                   double theVMin,
                                   double theVMax,
                                   double theTol) const
  {
    return Box(theUMin, theUMax, theVMin, theVMax, theTol);
  }

  //! Compute optimal bounding box for full surface.
  [[nodiscard]] Bnd_Box BoxOptimal(double theTol) const { return Box(theTol); }

private:
  occ::handle<Geom_CylindricalSurface> myGeom;
};

#endif // GeomBndLib_Cylinder_HeaderFile
