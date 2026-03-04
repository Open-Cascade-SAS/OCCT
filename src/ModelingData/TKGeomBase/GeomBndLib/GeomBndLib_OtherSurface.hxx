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

#ifndef GeomBndLib_OtherSurface_HeaderFile
#define GeomBndLib_OtherSurface_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Bnd_Box.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <functional>

//! Computes bounding box for a general surface via adaptor.
//! Uses grid sampling for Add and PSO/Powell numerical optimization for AddOptimal.
class GeomBndLib_OtherSurface
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_OtherSurface(const Adaptor3d_Surface& theSurf)
      : mySurf(theSurf)
  {
  }

  GeomBndLib_OtherSurface(const GeomBndLib_OtherSurface&)            = delete;
  GeomBndLib_OtherSurface& operator=(const GeomBndLib_OtherSurface&) = delete;
  GeomBndLib_OtherSurface(GeomBndLib_OtherSurface&&)                 = delete;
  GeomBndLib_OtherSurface& operator=(GeomBndLib_OtherSurface&&)      = delete;

  //! Add bounding box for full surface.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for surface patch [theUMin, theUMax] x [theVMin, theVMax].
  Standard_EXPORT void Add(double   theUMin,
                           double   theUMax,
                           double   theVMin,
                           double   theVMax,
                           double   theTol,
                           Bnd_Box& theBox) const;

  //! Add precise bounding box for full surface.
  void AddOptimal(double theTol, Bnd_Box& theBox) const
  {
    AddOptimal(mySurf.get().FirstUParameter(),
               mySurf.get().LastUParameter(),
               mySurf.get().FirstVParameter(),
               mySurf.get().LastVParameter(),
               theTol,
               theBox);
  }

  //! Add precise bounding box using PSO + Powell optimization.
  Standard_EXPORT void AddOptimal(double   theUMin,
                                  double   theUMax,
                                  double   theVMin,
                                  double   theVMax,
                                  double   theTol,
                                  Bnd_Box& theBox) const;

private:
  std::reference_wrapper<const Adaptor3d_Surface> mySurf;
};

#endif // GeomBndLib_OtherSurface_HeaderFile
