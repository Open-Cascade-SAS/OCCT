// Created on: 1992-04-03
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

#ifndef _IntRes2d_IntersectionSegment_HeaderFile
#define _IntRes2d_IntersectionSegment_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntRes2d_IntersectionPoint.hxx>

//! Definition of an intersection curve between
//! two 2D curves.
class IntRes2d_IntersectionSegment
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor.
  Standard_EXPORT IntRes2d_IntersectionSegment();

  IntRes2d_IntersectionSegment(const IntRes2d_IntersectionPoint& P1,
                               const IntRes2d_IntersectionPoint& P2,
                               const Standard_Boolean            Oppos,
                               const Standard_Boolean            ReverseFlag);

  IntRes2d_IntersectionSegment(const IntRes2d_IntersectionPoint& P,
                               const Standard_Boolean            First,
                               const Standard_Boolean            Oppos,
                               const Standard_Boolean            ReverseFlag);

  //! Creates an infinite segment of intersection.
  IntRes2d_IntersectionSegment(const Standard_Boolean Oppos);

  //! Returns FALSE if the intersection segment has got
  //! the same orientation on both curves.
  Standard_Boolean IsOpposite() const;

  //! Returns True if the segment is  limited by a first
  //! point.   This  point defines  the lowest parameter
  //! admitted on the first  curve for the  segment.  If
  //! IsOpposite  returns  False, it  defines the lowest
  //! parameter on the  second curve, otherwise,  it  is
  //! the highest parameter on the second curve.
  Standard_Boolean HasFirstPoint() const;

  //! Returns the  first point   of the segment    as an
  //! IntersectionPoint (with    a    transition).   The
  //! exception  DomainError  is raised if HasFirstPoint
  //! returns False.
  const IntRes2d_IntersectionPoint& FirstPoint() const;

  //! Returns True if the segment  is  limited by a last
  //! point.  This point  defines  the highest parameter
  //! admitted on the  first curve for  the segment.  If
  //! IsOpposite returns  False, it  defines the highest
  //! parameter on  the  second curve, otherwise, it  is
  //! the lowest parameter on the second curve.
  Standard_Boolean HasLastPoint() const;

  //! Returns   the  last point  of the    segment as an
  //! IntersectionPoint   (with  a    transition).   The
  //! exception        DomainError     is   raised    if
  //! HasLastExtremity returns False.
  const IntRes2d_IntersectionPoint& LastPoint() const;

protected:
private:
  Standard_Boolean           oppos;
  Standard_Boolean           first;
  Standard_Boolean           last;
  IntRes2d_IntersectionPoint ptfirst;
  IntRes2d_IntersectionPoint ptlast;
};

#include <IntRes2d_IntersectionSegment.lxx>

#endif // _IntRes2d_IntersectionSegment_HeaderFile
