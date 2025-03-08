// Created on: 1992-04-27
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IntRes2d_Intersection_HeaderFile
#define _IntRes2d_Intersection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntRes2d_SequenceOfIntersectionPoint.hxx>
#include <IntRes2d_SequenceOfIntersectionSegment.hxx>
class IntRes2d_IntersectionPoint;
class IntRes2d_IntersectionSegment;

//! Defines  the root   class  of  all  the  Intersections
//! between  two 2D-Curves, and  provides all  the methods
//! about the results of the Intersections Algorithms.
class IntRes2d_Intersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! returns TRUE when the computation was successful.
  Standard_Boolean IsDone() const;

  //! Returns TRUE if there is no intersection between the
  //! given arguments.
  //! The exception NotDone is raised if IsDone returns FALSE.
  Standard_Boolean IsEmpty() const;

  //! This function returns the number of intersection
  //! points between the 2 curves.
  //! The exception NotDone is raised if IsDone returns FALSE.
  Standard_Integer NbPoints() const;

  //! This function returns the intersection point
  //! of range N;
  //! The exception NotDone is raised if IsDone returns FALSE.
  //! The exception OutOfRange is raised if (N <= 0)
  //! or (N > NbPoints).
  const IntRes2d_IntersectionPoint& Point(const Standard_Integer N) const;

  //! This function returns the number of intersection
  //! segments between the two curves.
  //! The exception NotDone is raised if IsDone returns FALSE.
  Standard_Integer NbSegments() const;

  //! This function returns the intersection segment
  //! of range N;
  //! The exception NotDone is raised if IsDone returns FALSE.
  //! The exception OutOfRange is raised if (N <= 0)
  //! or (N > NbPoints).
  const IntRes2d_IntersectionSegment& Segment(const Standard_Integer N) const;

  void SetReversedParameters(const Standard_Boolean Reverseflag);

protected:
  //! Empty constructor.
  IntRes2d_Intersection();

  IntRes2d_Intersection(const IntRes2d_Intersection& Other);

  //! Assignment
  IntRes2d_Intersection& operator=(const IntRes2d_Intersection& theOther)
  {
    done    = theOther.done;
    reverse = theOther.reverse;
    lpnt    = theOther.lpnt;
    lseg    = theOther.lseg;
    return *this;
  }

  //! Destructor is protected, for safe inheritance
  ~IntRes2d_Intersection() {}

  Standard_EXPORT void SetValues(const IntRes2d_Intersection& Inter);

  Standard_EXPORT void Append(const IntRes2d_Intersection& Inter,
                              const Standard_Real          FirstParam1,
                              const Standard_Real          LastParam1,
                              const Standard_Real          FirstParam2,
                              const Standard_Real          LastParam2);

  void Append(const IntRes2d_IntersectionSegment& Seg);

  void Append(const IntRes2d_IntersectionPoint& Pnt);

  Standard_EXPORT void Insert(const IntRes2d_IntersectionPoint& Pnt);

  void ResetFields();

  Standard_Boolean ReversedParameters() const;

protected:
  IntRes2d_SequenceOfIntersectionPoint   lpnt;
  IntRes2d_SequenceOfIntersectionSegment lseg;
  Standard_Boolean                       done;
  Standard_Boolean                       reverse;
};

#include <IntRes2d_Intersection.lxx>

#endif // _IntRes2d_Intersection_HeaderFile
