// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Contap_TheIWLineOfTheIWalking_HeaderFile
#define _Contap_TheIWLineOfTheIWalking_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IntSurf_SequenceOfCouple.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <IntSurf_PathPoint.hxx>
#include <gp_Vec.hxx>
#include <MMgt_TShared.hxx>
#include <IntSurf_Allocator.hxx>
class IntSurf_LineOn2S;
class Standard_OutOfRange;
class Standard_DomainError;
class IntSurf_PathPoint;
class IntSurf_PntOn2S;
class gp_Vec;


class Contap_TheIWLineOfTheIWalking;
DEFINE_STANDARD_HANDLE(Contap_TheIWLineOfTheIWalking, MMgt_TShared)


class Contap_TheIWLineOfTheIWalking : public MMgt_TShared
{

public:

  
  Standard_EXPORT Contap_TheIWLineOfTheIWalking(const IntSurf_Allocator& theAllocator = 0);
  
  Standard_EXPORT void Reverse();
  
    void Cut (const Standard_Integer Index);
  
    void AddPoint (const IntSurf_PntOn2S& P);
  
    void AddStatusFirst (const Standard_Boolean Closed, const Standard_Boolean HasFirst);
  
    void AddStatusFirst (const Standard_Boolean Closed, const Standard_Boolean HasLast, const Standard_Integer Index, const IntSurf_PathPoint& P);
  
    void AddStatusFirstLast (const Standard_Boolean Closed, const Standard_Boolean HasFirst, const Standard_Boolean HasLast);
  
    void AddStatusLast (const Standard_Boolean HasLast);
  
    void AddStatusLast (const Standard_Boolean HasLast, const Standard_Integer Index, const IntSurf_PathPoint& P);
  
    void AddIndexPassing (const Standard_Integer Index);
  
    void SetTangentVector (const gp_Vec& V, const Standard_Integer Index);
  
    void SetTangencyAtBegining (const Standard_Boolean IsTangent);
  
    void SetTangencyAtEnd (const Standard_Boolean IsTangent);
  
    Standard_Integer NbPoints() const;
  
    const IntSurf_PntOn2S& Value (const Standard_Integer Index) const;
  
    const Handle(IntSurf_LineOn2S)& Line() const;
  
    Standard_Boolean IsClosed() const;
  
    Standard_Boolean HasFirstPoint() const;
  
    Standard_Boolean HasLastPoint() const;
  
    const IntSurf_PathPoint& FirstPoint() const;
  
    Standard_Integer FirstPointIndex() const;
  
    const IntSurf_PathPoint& LastPoint() const;
  
    Standard_Integer LastPointIndex() const;
  
    Standard_Integer NbPassingPoint() const;
  
    void PassingPoint (const Standard_Integer Index, Standard_Integer& IndexLine, Standard_Integer& IndexPnts) const;
  
    const gp_Vec& TangentVector (Standard_Integer& Index) const;
  
    Standard_Boolean IsTangentAtBegining() const;
  
    Standard_Boolean IsTangentAtEnd() const;




  DEFINE_STANDARD_RTTI(Contap_TheIWLineOfTheIWalking,MMgt_TShared)

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
#define IntWalk_IWLine Contap_TheIWLineOfTheIWalking
#define IntWalk_IWLine_hxx <Contap_TheIWLineOfTheIWalking.hxx>
#define Handle_IntWalk_IWLine Handle(Contap_TheIWLineOfTheIWalking)

#include <IntWalk_IWLine.lxx>

#undef TheStartPoint
#undef TheStartPoint_hxx
#undef IntWalk_IWLine
#undef IntWalk_IWLine_hxx
#undef Handle_IntWalk_IWLine




#endif // _Contap_TheIWLineOfTheIWalking_HeaderFile
