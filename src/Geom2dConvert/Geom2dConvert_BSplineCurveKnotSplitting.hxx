// Created on: 1991-10-03
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Geom2dConvert_BSplineCurveKnotSplitting_HeaderFile
#define _Geom2dConvert_BSplineCurveKnotSplitting_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Standard_DimensionError;
class Standard_RangeError;
class Geom2d_BSplineCurve;


//! An algorithm to determine points at which a BSpline
//! curve should be split in order to obtain arcs of the same continuity.
//! If you require curves with a minimum continuity for
//! your computation, it is useful to know the points
//! between which an arc has a continuity of a given
//! order. The continuity order is given at the construction time.
//! For a BSpline curve, the discontinuities are
//! localized at the knot values. Between two knot values
//! the BSpline is infinitely and continuously
//! differentiable. At a given knot, the continuity is equal
//! to: Degree - Mult, where Degree is the
//! degree of the BSpline curve and Mult is the multiplicity of the knot.
//! It is possible to compute the arcs which correspond to
//! this splitting using the global function
//! SplitBSplineCurve provided by the package Geom2dConvert.
//! A BSplineCurveKnotSplitting object provides a framework for:
//! -   defining the curve to be analysed and the required degree of continuity,
//! -   implementing the computation algorithm, and
//! -   consulting the results.
class Geom2dConvert_BSplineCurveKnotSplitting 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Determines points at which the BSpline curve
  //! BasisCurve should be split in order to obtain arcs
  //! with a degree of continuity equal to ContinuityRange.
  //! These points are knot values of BasisCurve. They
  //! are identified by indices in the knots table of BasisCurve.
  //! Use the available interrogation functions to access
  //! computed values, followed by the global function
  //! SplitBSplineCurve (provided by the package
  //! Geom2dConvert) to split the curve.
  //! Exceptions
  //! Standard_RangeError if ContinuityRange is less than zero.
  Standard_EXPORT Geom2dConvert_BSplineCurveKnotSplitting(const Handle(Geom2d_BSplineCurve)& BasisCurve, const Standard_Integer ContinuityRange);
  
  //! Returns the number of points at which the analysed
  //! BSpline curve should be split, in order to obtain arcs
  //! with the continuity required by this framework.
  //! All these points correspond to knot values. Note that
  //! the first and last points of the curve, which bound the
  //! first and last arcs, are counted among these splitting points.
  Standard_EXPORT Standard_Integer NbSplits() const;
  
  //! Loads the SplitValues table with the split knots
  //! values computed in this framework. Each value in the
  //! table is an index in the knots table of the BSpline
  //! curve analysed by this algorithm.
  //! The values in SplitValues are given in ascending
  //! order and comprise the indices of the knots which
  //! give the first and last points of the curve. Use two
  //! consecutive values from the table as arguments of the
  //! global function SplitBSplineCurve (provided by the
  //! package Geom2dConvert) to split the curve.
  //! Exceptions
  //! Standard_DimensionError if the array SplitValues
  //! was not created with the following bounds:
  //! -   1, and
  //! -   the number of split points computed in this
  //! framework (as given by the function NbSplits).
  Standard_EXPORT void Splitting (TColStd_Array1OfInteger& SplitValues) const;
  
  //! Returns the split knot of index Index to the split knots
  //! table computed in this framework. The returned value
  //! is an index in the knots table of the BSpline curve
  //! analysed by this algorithm.
  //! Notes:
  //! -   If Index is equal to 1, the corresponding knot
  //! gives the first point of the curve.
  //! -   If Index is equal to the number of split knots
  //! computed in this framework, the corresponding
  //! point is the last point of the curve.
  //! Exceptions
  //! Standard_RangeError if Index is less than 1 or
  //! greater than the number of split knots computed in this framework.
  Standard_EXPORT Standard_Integer SplitValue (const Standard_Integer Index) const;




protected:





private:



  Handle(TColStd_HArray1OfInteger) splitIndexes;


};







#endif // _Geom2dConvert_BSplineCurveKnotSplitting_HeaderFile
