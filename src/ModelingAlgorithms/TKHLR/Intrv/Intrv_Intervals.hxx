// Created on: 1991-12-13
// Created by: Christophe MARION
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

#ifndef _Intrv_Intervals_HeaderFile
#define _Intrv_Intervals_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Intrv_SequenceOfInterval.hxx>
class Intrv_Interval;

//! The class  Intervals is a  sorted  sequence of non
//! overlapping  Real Intervals.
class Intrv_Intervals
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a void sequence of intervals.
  Standard_EXPORT Intrv_Intervals();

  //! Creates a sequence of one interval.
  Standard_EXPORT Intrv_Intervals(const Intrv_Interval& Int);

  //! Intersects the intervals with the interval <Tool>.
  Standard_EXPORT void Intersect(const Intrv_Interval& Tool);

  //! Intersects the intervals with the intervals in the
  //! sequence  <Tool>.
  Standard_EXPORT void Intersect(const Intrv_Intervals& Tool);

  Standard_EXPORT void Subtract(const Intrv_Interval& Tool);

  Standard_EXPORT void Subtract(const Intrv_Intervals& Tool);

  Standard_EXPORT void Unite(const Intrv_Interval& Tool);

  Standard_EXPORT void Unite(const Intrv_Intervals& Tool);

  Standard_EXPORT void XUnite(const Intrv_Interval& Tool);

  Standard_EXPORT void XUnite(const Intrv_Intervals& Tool);

  Standard_Integer NbIntervals() const;

  const Intrv_Interval& Value(const Standard_Integer Index) const;

protected:
private:
  Intrv_SequenceOfInterval myInter;
};

#include <Intrv_Intervals.lxx>

#endif // _Intrv_Intervals_HeaderFile
