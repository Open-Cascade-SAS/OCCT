// Created on: 1996-06-11
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _LocOpe_CSIntersector_HeaderFile
#define _LocOpe_CSIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <LocOpe_SequenceOfLin.hxx>
#include <LocOpe_SequenceOfCirc.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class TopoDS_Shape;
class LocOpe_PntFace;


//! This class provides the intersection between a set
//! of axis or a circle and the faces of a shape.  The
//! intersection  points  are   sorted  in  increasing
//! parameter along each axis or circle.
class LocOpe_CSIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    LocOpe_CSIntersector();
  
  //! Creates  and performs the intersection     betwwen
  //! <Ax1> and <S>.
    LocOpe_CSIntersector(const TopoDS_Shape& S);
  
  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init (const TopoDS_Shape& S);
  
  Standard_EXPORT void Perform (const LocOpe_SequenceOfLin& Slin);
  
  Standard_EXPORT void Perform (const LocOpe_SequenceOfCirc& Scir);
  
  Standard_EXPORT void Perform (const TColGeom_SequenceOfCurve& Scur);
  
  //! Returns <Standard_True>  if the  intersection  has
  //! been done.
    Standard_Boolean IsDone() const;
  
  //! Returns  the number of   intersection point on the
  //! element of range <I>.
  Standard_EXPORT Standard_Integer NbPoints (const Standard_Integer I) const;
  
  //! Returns the intersection point of range <Index> on
  //! element of range   <I>. The points   are sorted in
  //! increasing order of parameter along the axis.
  Standard_EXPORT const LocOpe_PntFace& Point (const Standard_Integer I, const Standard_Integer Index) const;
  
  //! On  the element of range   <I>, searches the first
  //! intersection   point  located after  the parameter
  //! <From>,  which orientation is not TopAbs_EXTERNAL.
  //! If  found, returns <Standard_True>.  <Or> contains
  //! the orientation    of  the  point,  <IndFrom>  and
  //! <IndTo> represents  the interval  of index in  the
  //! sequence  of intersection  point corresponding  to
  //! the  point. (IndFrom <=   IndTo). <Tol> is used to
  //! determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeAfter (const Standard_Integer I, const Standard_Real From, const Standard_Real Tol, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! On the element  of range  <I>, searches the  first
  //! intersection point   located before  the parameter
  //! <From>,  which orientation is not TopAbs_EXTERNAL.
  //! If found,  returns <Standard_True>.  <Or> contains
  //! the   orientation  of   the point,  <IndFrom>  and
  //! <IndTo> represents the interval   of index in  the
  //! sequence of  intersection  point corresponding  to
  //! the point (IndFrom  <=  IndTo). <Tol> is   used to
  //! determine if 2 parameters are equal.
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeBefore (const Standard_Integer I, const Standard_Real From, const Standard_Real Tol, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! On the  element of  range <I>, searches  the first
  //! intersection      point  located after the   index
  //! <FromInd> ( >=  FromInd + 1), which orientation is
  //! not    TopAbs_EXTERNAL.      If    found,  returns
  //! <Standard_True>.  <Or> contains the orientation of
  //! the  point, <IndFrom>  and <IndTo> represents  the
  //! interval of index in  the sequence of intersection
  //! point corresponding   to the  point.  (IndFrom  <=
  //! IndTo). <Tol> is used to determine if 2 parameters
  //! are equal.
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeAfter (const Standard_Integer I, const Standard_Integer FromInd, const Standard_Real Tol, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! On  the element of  range  <I>, searches the first
  //! intersection  point    located  before  the  index
  //! <FromInd>  (  <= FromInd -1), which orientation is
  //! not   TopAbs_EXTERNAL.  If    found,       returns
  //! <Standard_True>.  <Or> contains the orientation of
  //! the  point, <IndFrom>  and  <IndTo> represents the
  //! interval of  index in the sequence of intersection
  //! point  corresponding to   the  point  (IndFrom  <=
  //! IndTo). <Tol> is used to determine if 2 parameters
  //! are equal.
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeBefore (const Standard_Integer I, const Standard_Integer FromInd, const Standard_Real Tol, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  Standard_EXPORT void Destroy();
~LocOpe_CSIntersector()
{
  Destroy();
}




protected:





private:



  Standard_Boolean myDone;
  TopoDS_Shape myShape;
  Standard_Address myPoints;
  Standard_Integer myNbelem;


};


#include <LocOpe_CSIntersector.lxx>





#endif // _LocOpe_CSIntersector_HeaderFile
