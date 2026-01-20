// Created on: 1994-02-17
// Created by: Bruno DUMORTIER
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

#ifndef _GeomFill_Profiler_HeaderFile
#define _GeomFill_Profiler_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
class Geom_Curve;

//! Evaluation of the common BSplineProfile of a group
//! of curves from Geom. All the curves will have the
//! same degree, the same knot-vector, so the same
//! number of poles.
class GeomFill_Profiler
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_Profiler();
  Standard_EXPORT virtual ~GeomFill_Profiler();

  Standard_EXPORT void AddCurve(const occ::handle<Geom_Curve>& Curve);

  //! Converts all curves to BSplineCurves.
  //! Set them to the common profile.
  //! <PTol> is used to compare 2 knots.
  Standard_EXPORT virtual void Perform(const double PTol);

  //! Raises if not yet perform
  Standard_EXPORT int Degree() const;

  bool IsPeriodic() const;

  //! Raises if not yet perform
  Standard_EXPORT int NbPoles() const;

  //! returns in <Poles> the poles of the BSplineCurve
  //! from index <Index> adjusting to the current profile.
  //! Raises if not yet perform
  //! Raises if <Index> not in the range [1,NbCurves]
  //! if the length of <Poles> is not equal to
  //! NbPoles().
  Standard_EXPORT void Poles(const int Index, NCollection_Array1<gp_Pnt>& Poles) const;

  //! returns in <Weights> the weights of the BSplineCurve
  //! from index <Index> adjusting to the current profile.
  //! Raises if not yet perform
  //! Raises if <Index> not in the range [1,NbCurves] or
  //! if the length of <Weights> is not equal to
  //! NbPoles().
  Standard_EXPORT void Weights(const int Index, NCollection_Array1<double>& Weights) const;

  //! Raises if not yet perform
  Standard_EXPORT int NbKnots() const;

  //! Raises if not yet perform
  //! Raises if the lengths of <Knots> and <Mults> are
  //! not equal to NbKnots().
  Standard_EXPORT void KnotsAndMults(NCollection_Array1<double>&    Knots,
                                     NCollection_Array1<int>& Mults) const;

  const occ::handle<Geom_Curve>& Curve(const int Index) const;

protected:
  NCollection_Sequence<occ::handle<Geom_Curve>> mySequence;
  bool         myIsDone;
  bool         myIsPeriodic;

};

#include <GeomFill_Profiler.lxx>

#endif // _GeomFill_Profiler_HeaderFile
