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

#ifndef GeomBndLib_Cone_HeaderFile
#define GeomBndLib_Cone_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a conical surface (Geom_ConicalSurface).
//! Uses ElSLib iso-curves and GeomBndLib_ConicHelpers for circle arc bounding.
class GeomBndLib_Cone
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Cone(const occ::handle<Geom_ConicalSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_Cone(const GeomBndLib_Cone&)            = delete;
  GeomBndLib_Cone& operator=(const GeomBndLib_Cone&) = delete;
  GeomBndLib_Cone(GeomBndLib_Cone&&)                 = delete;
  GeomBndLib_Cone& operator=(GeomBndLib_Cone&&)      = delete;

  const occ::handle<Geom_ConicalSurface>& Geometry() const { return myGeom; }

  //! Add bounding box for full cone.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for cone patch [theUMin, theUMax] x [theVMin, theVMax].
  Standard_EXPORT void Add(double   theUMin,
                           double   theUMax,
                           double   theVMin,
                           double   theVMax,
                           double   theTol,
                           Bnd_Box& theBox) const;

  //! For analytical surfaces, AddOptimal is same as Add.
  void AddOptimal(double   theUMin,
                  double   theUMax,
                  double   theVMin,
                  double   theVMax,
                  double   theTol,
                  Bnd_Box& theBox) const
  {
    Add(theUMin, theUMax, theVMin, theVMax, theTol, theBox);
  }

  //! AddOptimal for full surface.
  void AddOptimal(double theTol, Bnd_Box& theBox) const { Add(theTol, theBox); }

private:
  occ::handle<Geom_ConicalSurface> myGeom;
};

#endif // GeomBndLib_Cone_HeaderFile
