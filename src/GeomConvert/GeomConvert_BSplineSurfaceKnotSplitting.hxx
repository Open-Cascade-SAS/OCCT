// Created on: 1991-10-04
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

#ifndef _GeomConvert_BSplineSurfaceKnotSplitting_HeaderFile
#define _GeomConvert_BSplineSurfaceKnotSplitting_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfInteger.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfInteger.hxx>
class Standard_DimensionError;
class Standard_RangeError;
class Geom_BSplineSurface;


//! An algorithm to determine isoparametric curves along
//! which a BSpline surface should be split in order to
//! obtain patches of the same continuity. The continuity order is given at the
//! construction time. It is possible to compute the surface patches
//! corresponding to the splitting with the method of package
//! SplitBSplineSurface.
//! For a B-spline surface the discontinuities are localised at
//! the knot values. Between two knots values the B-spline is
//! infinitely continuously differentiable.  For each parametric
//! direction at a knot of range index the continuity in this
//! direction is equal to :  Degree - Mult (Index)   where  Degree
//! is the degree of the basis B-spline functions and Mult the
//! multiplicity of the knot of range Index in the given direction.
//! If for your computation you need to have B-spline surface with a
//! minima of continuity it can be interesting to know between which
//! knot values, a B-spline patch, has a continuity of given order.
//! This algorithm computes the indexes of the knots where you should
//! split the surface, to obtain patches with a constant continuity
//! given at the construction time. If you just want to compute the
//! local derivatives on the surface you don't need to create the
//! BSpline patches, you can use the functions LocalD1, LocalD2,
//! LocalD3, LocalDN of the class BSplineSurface from package Geom.
class GeomConvert_BSplineSurfaceKnotSplitting 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Determines the u- and v-isoparametric curves
  //! along which the BSpline surface BasisSurface
  //! should be split in order to obtain patches with a
  //! degree of continuity equal to UContinuityRange in
  //! the u parametric direction, and to
  //! VContinuityRange in the v parametric direction.
  //! These isoparametric curves are defined by
  //! parameters, which are BasisSurface knot values in
  //! the u or v parametric direction. They are identified
  //! by indices in the BasisSurface knots table in the
  //! corresponding parametric direction.
  //! Use the available interrogation functions to access
  //! computed values, followed by the global function
  //! SplitBSplineSurface (provided by the package
  //! GeomConvert) to split the surface.
  //! Exceptions
  //! Standard_RangeError if UContinuityRange or
  //! VContinuityRange is less than zero.
  Standard_EXPORT GeomConvert_BSplineSurfaceKnotSplitting(const Handle(Geom_BSplineSurface)& BasisSurface, const Standard_Integer UContinuityRange, const Standard_Integer VContinuityRange);
  
  //! Returns the number of u-isoparametric curves
  //! along which the analysed BSpline surface should be
  //! split in order to obtain patches with the continuity
  //! required by this framework.
  //! The parameters which define these curves are knot
  //! values in the corresponding parametric direction.
  //! Note that the four curves which bound the surface are
  //! counted among these splitting curves.
  Standard_EXPORT Standard_Integer NbUSplits() const;
  
  //! Returns the number of v-isoparametric curves
  //! along which the analysed BSpline surface should be
  //! split in order to obtain patches with the continuity
  //! required by this framework.
  //! The parameters which define these curves are knot
  //! values in the corresponding parametric direction.
  //! Note that the four curves which bound the surface are
  //! counted among these splitting curves.
  Standard_EXPORT Standard_Integer NbVSplits() const;
  
  //! Loads the USplit and VSplit tables with the split
  //! knots values computed in this framework. Each value
  //! in these tables is an index in the knots table
  //! corresponding to the u or v parametric direction of
  //! the BSpline surface analysed by this algorithm.
  //! The USplit and VSplit values are given in ascending
  //! order and comprise the indices of the knots which
  //! give the first and last isoparametric curves of the
  //! surface in the corresponding parametric direction.
  //! Use two consecutive values from the USplit table and
  //! two consecutive values from the VSplit table as
  //! arguments of the global function
  //! SplitBSplineSurface (provided by the package
  //! GeomConvert) to split the surface.
  //! Exceptions
  //! Standard_DimensionError if:
  //! -   the array USplit was not created with the following bounds:
  //! -   1 , and
  //! -   the number of split knots in the u parametric
  //! direction computed in this framework (as given
  //! by the function NbUSplits); or
  //! -   the array VSplit was not created with the following bounds:
  //! -   1 , and
  //! -   the number of split knots in the v parametric
  //! direction computed in this framework (as given
  //! by the function NbVSplits).
  Standard_EXPORT void Splitting (TColStd_Array1OfInteger& USplit, TColStd_Array1OfInteger& VSplit) const;
  
  //! Returns the split knot of index UIndex
  //! to the split knots table for the u  parametric direction
  //! computed in this framework. The returned value is
  //! an index in the knots table relative to the u
  //! parametric direction of the BSpline surface analysed by this algorithm.
  //! Note: If UIndex is equal to 1, or to the number of split knots for the u
  //! parametric direction computed in
  //! this framework, the corresponding knot gives the
  //! parameter of one of the bounding curves of the surface.
  //! Exceptions
  //! Standard_RangeError if UIndex  is less than 1 or greater than the number
  //! of split knots for the u parametric direction computed in this framework.
  Standard_EXPORT Standard_Integer USplitValue (const Standard_Integer UIndex) const;
  
  //! Returns the split knot of index VIndex
  //! to the split knots table for the v  parametric direction
  //! computed in this framework. The returned value is
  //! an index in the knots table relative to the v
  //! parametric direction of the BSpline surface analysed by this algorithm.
  //! Note: If UIndex is equal to 1, or to the number of split knots for the v
  //! parametric direction computed in
  //! this framework, the corresponding knot gives the
  //! parameter of one of the bounding curves of the surface.
  //! Exceptions
  //! Standard_RangeError if VIndex  is less than 1 or greater than the number
  //! of split knots for the v parametric direction computed in this framework.
  Standard_EXPORT Standard_Integer VSplitValue (const Standard_Integer VIndex) const;




protected:





private:



  Handle(TColStd_HArray1OfInteger) usplitIndexes;
  Handle(TColStd_HArray1OfInteger) vsplitIndexes;


};







#endif // _GeomConvert_BSplineSurfaceKnotSplitting_HeaderFile
