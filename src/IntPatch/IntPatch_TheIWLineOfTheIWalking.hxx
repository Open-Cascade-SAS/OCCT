// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntPatch_TheIWLineOfTheIWalking_HeaderFile
#define _IntPatch_TheIWLineOfTheIWalking_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IntSurf_SequenceOfCouple.hxx>
#include <Standard_Integer.hxx>
#include <IntSurf_PathPoint.hxx>
#include <gp_Vec.hxx>
#include <Standard_Transient.hxx>
#include <IntSurf_Allocator.hxx>
class IntSurf_LineOn2S;
class Standard_OutOfRange;
class Standard_DomainError;
class IntSurf_PathPoint;
class IntSurf_PntOn2S;
class gp_Vec;


class IntPatch_TheIWLineOfTheIWalking;
DEFINE_STANDARD_HANDLE(IntPatch_TheIWLineOfTheIWalking, Standard_Transient)


class IntPatch_TheIWLineOfTheIWalking : public Standard_Transient
{

public:

  
  Standard_EXPORT IntPatch_TheIWLineOfTheIWalking(const IntSurf_Allocator& theAllocator = 0);
  
  //! reverse the points in the line. Hasfirst, HasLast are kept.
  Standard_EXPORT void Reverse();
  
  //! Cut the line at the point of rank Index.
    void Cut (const Standard_Integer Index);
  
  //! Add a point in the line.
    void AddPoint (const IntSurf_PntOn2S& P);
  
    void AddStatusFirst (const Standard_Boolean Closed, const Standard_Boolean HasFirst);
  
    void AddStatusFirst (const Standard_Boolean Closed, const Standard_Boolean HasLast, const Standard_Integer Index, const IntSurf_PathPoint& P);
  
    void AddStatusFirstLast (const Standard_Boolean Closed, const Standard_Boolean HasFirst, const Standard_Boolean HasLast);
  
    void AddStatusLast (const Standard_Boolean HasLast);
  
    void AddStatusLast (const Standard_Boolean HasLast, const Standard_Integer Index, const IntSurf_PathPoint& P);
  
  //! associer a l 'indice du point sur la ligne l'indice du point
  //! passant dans l'iterateur de depart
    void AddIndexPassing (const Standard_Integer Index);
  
    void SetTangentVector (const gp_Vec& V, const Standard_Integer Index);
  
    void SetTangencyAtBegining (const Standard_Boolean IsTangent);
  
    void SetTangencyAtEnd (const Standard_Boolean IsTangent);
  
  //! Returns the number of points of the line (including first
  //! point and end point : see HasLastPoint and HasFirstPoint).
    Standard_Integer NbPoints() const;
  
  //! Returns the point of range Index.
  //! If index <= 0 or Index > NbPoints, an exception is raised.
    const IntSurf_PntOn2S& Value (const Standard_Integer Index) const;
  
  //! Returns the LineOn2S contained in the walking line.
    const Handle(IntSurf_LineOn2S)& Line() const;
  
  //! Returns True if the line is closed.
    Standard_Boolean IsClosed() const;
  
  //! Returns True if the first point of the line is a
  //! marching point . when is HasFirstPoint==False ,the line
  //! begins on the natural bound of the surface.the line can be
  //! too long
    Standard_Boolean HasFirstPoint() const;
  
  //! Returns True if the end point of the line is a
  //! marching point (Point from IntWS).
  //! when is HasFirstPoint==False ,the line ends
  //! on the natural bound of the surface.the line can be
  //! too long.
    Standard_Boolean HasLastPoint() const;
  
  //! Returns the first point of the line when it is a
  //! marching point.
  //! An exception is raised if HasFirstPoint returns False.
    const IntSurf_PathPoint& FirstPoint() const;
  
  //! Returns the Index of first point of the line when it is a
  //! marching point.This index is the index in the
  //! PointStartIterator.
  //! An exception is raised if HasFirstPoint returns False.
    Standard_Integer FirstPointIndex() const;
  
  //! Returns the last point of the line when it is a
  //! marching point.
  //! An exception is raised if HasLastPoint returns False.
    const IntSurf_PathPoint& LastPoint() const;
  
  //! Returns the index of last point of the line when it is a
  //! marching point.This index is the index in the
  //! PointStartIterator.
  //! An exception is raised if HasLastPoint returns False.
    Standard_Integer LastPointIndex() const;
  
  //! returns the number of points belonging to Pnts1 which are
  //! passing point.
    Standard_Integer NbPassingPoint() const;
  
  //! returns the index of the point belonging to the line which
  //! is associated to the passing point belonging to Pnts1
  //! an exception is raised if Index > NbPassingPoint()
    void PassingPoint (const Standard_Integer Index, Standard_Integer& IndexLine, Standard_Integer& IndexPnts) const;
  
    const gp_Vec& TangentVector (Standard_Integer& Index) const;
  
    Standard_Boolean IsTangentAtBegining() const;
  
    Standard_Boolean IsTangentAtEnd() const;




  DEFINE_STANDARD_RTTI_INLINE(IntPatch_TheIWLineOfTheIWalking,Standard_Transient)

protected:




private:


  Handle(IntSurf_LineOn2S) line;
  IntSurf_SequenceOfCouple couple;
  Standard_Boolean closed;
  Standard_Boolean hasFirst;
  Standard_Boolean hasLast;
  Standard_Integer firstIndex;
  Standard_Integer lastIndex;
  IntSurf_PathPoint theFirstPoint;
  IntSurf_PathPoint theLastPoint;
  Standard_Integer indextg;
  gp_Vec vcttg;
  Standard_Boolean istgtbeg;
  Standard_Boolean istgtend;


};

#define TheStartPoint IntSurf_PathPoint
#define TheStartPoint_hxx <IntSurf_PathPoint.hxx>
#define IntWalk_IWLine IntPatch_TheIWLineOfTheIWalking
#define IntWalk_IWLine_hxx <IntPatch_TheIWLineOfTheIWalking.hxx>
#define Handle_IntWalk_IWLine Handle(IntPatch_TheIWLineOfTheIWalking)

#include <IntWalk_IWLine.lxx>

#undef TheStartPoint
#undef TheStartPoint_hxx
#undef IntWalk_IWLine
#undef IntWalk_IWLine_hxx
#undef Handle_IntWalk_IWLine




#endif // _IntPatch_TheIWLineOfTheIWalking_HeaderFile
