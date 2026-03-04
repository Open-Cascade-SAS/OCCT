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

#ifndef GeomBndLib_Torus_HeaderFile
#define GeomBndLib_Torus_HeaderFile

#include <Bnd_Box.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Computes bounding box for a toroidal surface (Geom_ToroidalSurface).
//! Uses 8-point polygon approximation via GeomBndLib_ConicHelpers and
//! extremal-point computation for degenerate torus (Ra < Ri).
class GeomBndLib_Torus
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_Torus(const occ::handle<Geom_ToroidalSurface>& theSurf)
      : myGeom(theSurf)
  {
  }

  GeomBndLib_Torus(const GeomBndLib_Torus&)            = delete;
  GeomBndLib_Torus& operator=(const GeomBndLib_Torus&) = delete;
  GeomBndLib_Torus(GeomBndLib_Torus&&)                 = delete;
  GeomBndLib_Torus& operator=(GeomBndLib_Torus&&)      = delete;

  const occ::handle<Geom_ToroidalSurface>& Geometry() const { return myGeom; }

  //! Add bounding box for full torus.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for torus patch [theUMin, theUMax] x [theVMin, theVMax].
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
  occ::handle<Geom_ToroidalSurface> myGeom;
};

#endif // GeomBndLib_Torus_HeaderFile
