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

#ifndef GeomBndLib_OtherCurve2d_HeaderFile
#define GeomBndLib_OtherCurve2d_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Bnd_Box2d.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <functional>

//! Computes bounding box for a general 2D curve via adaptor.
//! Uses sampling + PSO/Brent numerical optimization for AddOptimal.
class GeomBndLib_OtherCurve2d
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_OtherCurve2d(const Adaptor2d_Curve2d& theCurve)
      : myCurve(theCurve)
  {
  }

  GeomBndLib_OtherCurve2d(const GeomBndLib_OtherCurve2d&)            = delete;
  GeomBndLib_OtherCurve2d& operator=(const GeomBndLib_OtherCurve2d&) = delete;
  GeomBndLib_OtherCurve2d(GeomBndLib_OtherCurve2d&&)                 = delete;
  GeomBndLib_OtherCurve2d& operator=(GeomBndLib_OtherCurve2d&&)      = delete;

  //! Add bounding box for full curve.
  Standard_EXPORT void Add(double theTol, Bnd_Box2d& theBox) const;

  //! Add bounding box for arc [theU1, theU2].
  Standard_EXPORT void Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const;

  //! Add precise bounding box for full curve.
  void AddOptimal(double theTol, Bnd_Box2d& theBox) const
  {
    AddOptimal(myCurve.get().FirstParameter(), myCurve.get().LastParameter(), theTol, theBox);
  }

  //! Add precise bounding box using PSO + Brent optimization.
  Standard_EXPORT void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const;

private:
  std::reference_wrapper<const Adaptor2d_Curve2d> myCurve;
};

#endif // GeomBndLib_OtherCurve2d_HeaderFile
