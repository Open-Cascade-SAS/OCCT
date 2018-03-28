// Created on: 1993-04-07
// Created by: Laurent BUCHARD
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

#ifndef _IntCurveSurface_Intersection_HeaderFile
#define _IntCurveSurface_Intersection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <IntCurveSurface_SequenceOfPnt.hxx>
#include <IntCurveSurface_SequenceOfSeg.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class IntCurveSurface_IntersectionPoint;
class IntCurveSurface_IntersectionSegment;



class IntCurveSurface_Intersection 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the <done> field.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! returns the number of IntersectionPoint
  //! if IsDone returns True.
  //! else NotDone is raised.
  Standard_EXPORT Standard_Integer NbPoints() const;
  
  //! returns the IntersectionPoint of range <Index>
  //! raises NotDone if the computation has failed or if
  //! the computation has not been done
  //! raises OutOfRange if Index is not in the range <1..NbPoints>
  Standard_EXPORT const IntCurveSurface_IntersectionPoint& Point (const Standard_Integer Index) const;
  
  //! returns the number of IntersectionSegment
  //! if IsDone returns True.
  //! else NotDone is raised.
  Standard_EXPORT Standard_Integer NbSegments() const;
  
  //! returns the IntersectionSegment of range <Index>
  //! raises NotDone if the computation has failed or if
  //! the computation has not been done
  //! raises OutOfRange if Index is not in the range <1..NbSegment>
  Standard_EXPORT const IntCurveSurface_IntersectionSegment& Segment (const Standard_Integer Index) const;

  //! Returns true if curve is parallel or belongs surface
  //! This case is recognized only for some pairs 
  //! of analytical curves and surfaces (plane - line, ...)
  Standard_EXPORT Standard_Boolean IsParallel() const;

  //! Dump all the fields.
  Standard_EXPORT void Dump() const;

protected:
  
  //! Empty Constructor;
  Standard_EXPORT IntCurveSurface_Intersection();
  
  //! Destructor is protected, for safe inheritance
  ~IntCurveSurface_Intersection() {}

  //! Internal method
  //! copy the <Inter> fields to <me>
  Standard_EXPORT void SetValues (const IntCurveSurface_Intersection& Inter);
  
  //! Internal method
  //! Append the IntersectionPoints and
  //! IntersectionSegments of <Inter> to <me>.
  Standard_EXPORT void Append (const IntCurveSurface_Intersection& Inter, const Standard_Real FirstParamOnCurve, const Standard_Real LastParamOnCurve);
  
  //! Internal method
  //! Append the IntersectionPoints of <Inter> to <me>
  Standard_EXPORT void Append (const IntCurveSurface_IntersectionPoint& Pt);
  
  //! Internal method
  //! Append the IntersectionPoints of <Inter> to <me>
  Standard_EXPORT void Append (const IntCurveSurface_IntersectionSegment& Seg);
  
  //! Internal method
  //! Reset all the fields of <me>
  //! Clear the sequences of IntersectionPoints and Segments
  //! Set the field <done> to Standard_False.
  Standard_EXPORT void ResetFields();


  Standard_Boolean done;
  Standard_Boolean myIsParallel; //Curve is "parallel" surface
  //This case is recognized only for some pairs 
  //of analytical curves and surfaces (plane - line, ...)


private:



  IntCurveSurface_SequenceOfPnt lpnt;
  IntCurveSurface_SequenceOfSeg lseg;


};







#endif // _IntCurveSurface_Intersection_HeaderFile
