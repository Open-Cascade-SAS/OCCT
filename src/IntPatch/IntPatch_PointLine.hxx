// Created on: 2015-02-18
// Created by: Nikolai BUKHALOV
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#ifndef _IntPatch_PointLine_HeaderFile
#define _IntPatch_PointLine_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IntPatch_Line.hxx>
#include <Standard_Boolean.hxx>
#include <IntSurf_TypeTrans.hxx>
#include <IntSurf_Situation.hxx>
#include <Standard_Integer.hxx>
class Standard_DomainError;
class Standard_OutOfRange;
class IntSurf_PntOn2S;


class IntPatch_PointLine;
DEFINE_STANDARD_HANDLE(IntPatch_PointLine, IntPatch_Line)

//! Definition of an intersection line between two
//! surfaces.
//! A line defined by a set of points
//! (e.g. coming from a walking algorithm) as
//! defined in the class WLine or RLine (Restriction line).
class IntPatch_PointLine : public IntPatch_Line
{

public:

  
  //! Returns the number of intersection points.
  Standard_EXPORT virtual Standard_Integer NbPnts() const = 0;
  
  //! Returns the intersection point of range Index.
  Standard_EXPORT virtual const IntSurf_PntOn2S& Point (const Standard_Integer Index) const = 0;




  DEFINE_STANDARD_RTTI(IntPatch_PointLine,IntPatch_Line)

protected:

  
  //! To initialize the fields, when the transitions
  //! are In or Out.
  Standard_EXPORT IntPatch_PointLine(const Standard_Boolean Tang, const IntSurf_TypeTrans Trans1, const IntSurf_TypeTrans Trans2);
  
  //! To initialize the fields, when the transitions
  //! are Touch.
  Standard_EXPORT IntPatch_PointLine(const Standard_Boolean Tang, const IntSurf_Situation Situ1, const IntSurf_Situation Situ2);
  
  //! To initialize the fields, when the transitions
  //! are Undecided.
  Standard_EXPORT IntPatch_PointLine(const Standard_Boolean Tang);



private:




};







#endif // _IntPatch_PointLine_HeaderFile
