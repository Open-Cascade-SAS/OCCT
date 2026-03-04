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

#ifndef GeomBndLib_OtherCurve_HeaderFile
#define GeomBndLib_OtherCurve_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <functional>

//! Computes bounding box for a general 3D curve via adaptor.
//! Uses sampling + PSO/Brent numerical optimization for AddOptimal.
class GeomBndLib_OtherCurve
{
public:
  DEFINE_STANDARD_ALLOC

  GeomBndLib_OtherCurve(const Adaptor3d_Curve& theCurve)
      : myCurve(theCurve)
  {
  }

  GeomBndLib_OtherCurve(const GeomBndLib_OtherCurve&)            = delete;
  GeomBndLib_OtherCurve& operator=(const GeomBndLib_OtherCurve&) = delete;
  GeomBndLib_OtherCurve(GeomBndLib_OtherCurve&&)                 = delete;
  GeomBndLib_OtherCurve& operator=(GeomBndLib_OtherCurve&&)      = delete;

  //! Add bounding box for full curve.
  Standard_EXPORT void Add(double theTol, Bnd_Box& theBox) const;

  //! Add bounding box for arc [theU1, theU2].
  Standard_EXPORT void Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

  //! Add precise bounding box for full curve.
  void AddOptimal(double theTol, Bnd_Box& theBox) const
  {
    AddOptimal(myCurve.get().FirstParameter(), myCurve.get().LastParameter(), theTol, theBox);
  }

  //! Add precise bounding box using PSO + Brent optimization.
  Standard_EXPORT void AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const;

private:
  std::reference_wrapper<const Adaptor3d_Curve> myCurve;
};

#endif // GeomBndLib_OtherCurve_HeaderFile
