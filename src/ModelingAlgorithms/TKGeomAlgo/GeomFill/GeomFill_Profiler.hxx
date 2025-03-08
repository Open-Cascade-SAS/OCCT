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

#include <TColGeom_SequenceOfCurve.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Geom_Curve;

//! Evaluation of the common BSplineProfile of a group
//! of curves  from Geom. All the curves will have the
//! same  degree,  the same knot-vector, so  the  same
//! number of poles.
class GeomFill_Profiler
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomFill_Profiler();
  Standard_EXPORT virtual ~GeomFill_Profiler();

  Standard_EXPORT void AddCurve(const Handle(Geom_Curve)& Curve);

  //! Converts all curves to BSplineCurves.
  //! Set them to the common profile.
  //! <PTol> is used to compare 2 knots.
  Standard_EXPORT virtual void Perform(const Standard_Real PTol);

  //! Raises if not yet perform
  Standard_EXPORT Standard_Integer Degree() const;

  Standard_Boolean IsPeriodic() const;

  //! Raises if not yet perform
  Standard_EXPORT Standard_Integer NbPoles() const;

  //! returns in <Poles> the  poles  of the BSplineCurve
  //! from index <Index> adjusting to the current profile.
  //! Raises if not yet perform
  //! Raises if <Index> not in the range [1,NbCurves]
  //! if  the  length  of  <Poles>  is  not  equal  to
  //! NbPoles().
  Standard_EXPORT void Poles(const Standard_Integer Index, TColgp_Array1OfPnt& Poles) const;

  //! returns in <Weights> the weights of the BSplineCurve
  //! from index <Index> adjusting to the current profile.
  //! Raises if not yet perform
  //! Raises if <Index> not in the range [1,NbCurves] or
  //! if  the  length  of  <Weights>  is  not  equal  to
  //! NbPoles().
  Standard_EXPORT void Weights(const Standard_Integer Index, TColStd_Array1OfReal& Weights) const;

  //! Raises if not yet perform
  Standard_EXPORT Standard_Integer NbKnots() const;

  //! Raises if not yet perform
  //! Raises if  the lengths of <Knots> and <Mults> are
  //! not equal to NbKnots().
  Standard_EXPORT void KnotsAndMults(TColStd_Array1OfReal&    Knots,
                                     TColStd_Array1OfInteger& Mults) const;

  const Handle(Geom_Curve)& Curve(const Standard_Integer Index) const;

protected:
  TColGeom_SequenceOfCurve mySequence;
  Standard_Boolean         myIsDone;
  Standard_Boolean         myIsPeriodic;

private:
};

#include <GeomFill_Profiler.lxx>

#endif // _GeomFill_Profiler_HeaderFile
