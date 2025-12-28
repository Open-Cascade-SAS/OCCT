// Created on: 1994-09-23
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _MAT2d_CutCurve_HeaderFile
#define _MAT2d_CutCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom2d_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <MAT_Side.hxx>
#include <Standard_Integer.hxx>
class Geom2d_Curve;
class Geom2d_TrimmedCurve;

//! Cuts a curve at the extremas of curvature
//! and at the inflections. Constructs a trimmed
//! Curve for each interval.
class MAT2d_CutCurve
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT MAT2d_CutCurve();

  Standard_EXPORT MAT2d_CutCurve(const occ::handle<Geom2d_Curve>& C);

  //! Cuts a curve at the extremas of curvature
  //! and at the inflections.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& C);

  //! Cuts a curve at the inflections, and at the extremas
  //! of curvature where the concavity is on <aSide>.
  Standard_EXPORT void Perform(const occ::handle<Geom2d_Curve>& C, const MAT_Side aSide);

  //! Cuts a curve at the inflections.
  Standard_EXPORT void PerformInf(const occ::handle<Geom2d_Curve>& C);

  //! Returns True if the curve is not cut.
  Standard_EXPORT bool UnModified() const;

  //! Returns the number of curves.
  //! it's always greatest than 2.
  //!
  //! raises if the Curve is UnModified;
  Standard_EXPORT int NbCurves() const;

  //! Returns the Indexth curve.
  //! raises if Index not in the range [1,NbCurves()]
  Standard_EXPORT occ::handle<Geom2d_TrimmedCurve> Value(const int Index) const;

private:
  NCollection_Sequence<occ::handle<Geom2d_Curve>> theCurves;
};

#endif // _MAT2d_CutCurve_HeaderFile
