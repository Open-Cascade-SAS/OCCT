// Created on: 1994-04-07
// Created by: Isabelle GRIGNON
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

#ifndef _Law_HeaderFile
#define _Law_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Law_BSpFunc;
class Law_Linear;
class Law_BSpline;
class Adaptor3d_Curve;


//! Multiple services concerning 1d functions.
class Law 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! This algorithm searches the knot values corresponding to the
  //! splitting of a given B-spline law into  several arcs with
  //! the same continuity. The continuity order is given at the
  //! construction time.
  //! Builds a 1d bspline that   is near from Lin with  null
  //! derivatives at the extremities.
  Standard_EXPORT static Handle(Law_BSpFunc) MixBnd (const Handle(Law_Linear)& Lin);
  
  //! Builds  the poles of the 1d  bspline that is near from
  //! Lin with null derivatives at the extremities.
  Standard_EXPORT static Handle(TColStd_HArray1OfReal) MixBnd (const Standard_Integer Degree, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Handle(Law_Linear)& Lin);
  
  //! Builds the poles of the 1d bspline that is null on the
  //! right    side   of   Knots(Index)  (on  the    left if
  //! NulOnTheRight  is  false)    and   that is     like  a
  //! t*(1-t)(1-t) curve  on the  left side of  Knots(Index)
  //! (on the right  if NulOnTheRight is false).  The result
  //! curve is  C1 with  a derivative  equal  to 1. at first
  //! parameter (-1 at last  parameter  if  NulOnTheRight is
  //! false).
  //! Warning: Mults(Index) must greater or equal to degree-1.
  Standard_EXPORT static Handle(TColStd_HArray1OfReal) MixTgt (const Standard_Integer Degree, const TColStd_Array1OfReal& Knots, const TColStd_Array1OfInteger& Mults, const Standard_Boolean NulOnTheRight, const Standard_Integer Index);
  
  //! Computes a 1 d curve to  reparametrize a curve. Its an
  //! interpolation of NbPoints  points calculated  at quasi
  //! constant abscissa.
  Standard_EXPORT static Handle(Law_BSpline) Reparametrize (const Adaptor3d_Curve& Curve, const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasDF, const Standard_Boolean HasDL, const Standard_Real DFirst, const Standard_Real DLast, const Standard_Boolean Rev, const Standard_Integer NbPoints);
  
  //! Computes a 1  d curve to  scale  a field of  tangency.
  //! Value is 1. for t = (First+Last)/2 .
  //! If HasFirst value for t = First is VFirst (null derivative).
  //! If HasLast value for t = Last is VLast (null derivative).
  //!
  //! 1.                   _
  //! _/ \_
  //! __/     \__
  //! /           
  //! VFirst    ____/             
  //! VLast                        \____
  //! First                    Last
  Standard_EXPORT static Handle(Law_BSpline) Scale (const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasF, const Standard_Boolean HasL, const Standard_Real VFirst, const Standard_Real VLast);
  
  Standard_EXPORT static Handle(Law_BSpline) ScaleCub (const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasF, const Standard_Boolean HasL, const Standard_Real VFirst, const Standard_Real VLast);

};

#endif // _Law_HeaderFile
