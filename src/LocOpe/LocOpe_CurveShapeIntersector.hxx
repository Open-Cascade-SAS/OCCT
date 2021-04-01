// Created on: 1995-05-29
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _LocOpe_CurveShapeIntersector_HeaderFile
#define _LocOpe_CurveShapeIntersector_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <LocOpe_SequenceOfPntFace.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_Orientation.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class gp_Ax1;
class TopoDS_Shape;
class gp_Circ;
class LocOpe_PntFace;


//! This  class  provides  the intersection between an
//! axis  or  a circle and  the  faces of a shape. The
//! intersection   points  are   sorted in  increasing
//! parameter along the axis.
class LocOpe_CurveShapeIntersector 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
    LocOpe_CurveShapeIntersector();
  
  //! Creates  and performs the intersection     betwwen
  //! <Ax1> and <S>.
    LocOpe_CurveShapeIntersector(const gp_Ax1& Axis, const TopoDS_Shape& S);
  
  //! Creates  and performs yte intersection     betwwen
  //! <C> and <S>.
    LocOpe_CurveShapeIntersector(const gp_Circ& C, const TopoDS_Shape& S);
  
  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init (const gp_Ax1& Axis, const TopoDS_Shape& S);
  
  //! Performs the intersection between <Ax1 and <S>.
  Standard_EXPORT void Init (const gp_Circ& C, const TopoDS_Shape& S);
  
  //! Returns <Standard_True>  if the  intersection  has
  //! been done.
    Standard_Boolean IsDone() const;
  
  //! Returns the number of intersection point.
    Standard_Integer NbPoints() const;
  
  //! Returns the intersection  point  of range <Index>.
  //! The points  are   sorted in increasing  order   of
  //! parameter along the axis.
    const LocOpe_PntFace& Point (const Standard_Integer Index) const;
  
  //! Searches the   first intersection  point   located
  //! after the parameter  <From>, which  orientation is
  //! not       TopAbs_EXTERNAL.      If found,  returns
  //! <Standard_True>.  <Or> contains the orientation of
  //! the  point, <IndFrom>  and  <IndTo> represents the
  //! interval of index  in the sequence of intersection
  //! point  corresponding  to   the point. (IndFrom  <=
  //! IndTo).
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeAfter (const Standard_Real From, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! Searches  the first intersection point     located
  //! before  the parameter <From>, which orientation is
  //! not      TopAbs_EXTERNAL.      If  found,  returns
  //! <Standard_True>.  <Or> contains the orientation of
  //! the point,  <IndFrom>  and <IndTo>  represents the
  //! interval of index  in the sequence of intersection
  //! point  corresponding   to the point   (IndFrom  <=
  //! IndTo).
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeBefore (const Standard_Real From, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! Searches  the first intersection point     located
  //! after the index <FromInd> ( >= FromInd + 1), which
  //! orientation   is   not TopAbs_EXTERNAL.   If found,
  //! returns   <Standard_True>.   <Or>  contains    the
  //! orientation of the  point, <IndFrom>  and  <IndTo>
  //! represents the interval  of index in  the sequence
  //! of  intersection  point     corresponding to   the
  //! point. (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeAfter (const Standard_Integer FromInd, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;
  
  //! Searches the  first  intersection   point  located
  //! before the index <FromInd> ( <= FromInd -1), which
  //! orientation is   not TopAbs_EXTERNAL.   If   found,
  //! returns   <Standard_True>.  <Or>  contains     the
  //! orientation  of the  point,  <IndFrom> and <IndTo>
  //! represents the interval  of index  in the sequence
  //! of  intersection  point corresponding to the point
  //! (IndFrom <= IndTo).
  //!
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean LocalizeBefore (const Standard_Integer FromInd, TopAbs_Orientation& Or, Standard_Integer& IndFrom, Standard_Integer& IndTo) const;




protected:





private:



  Standard_Boolean myDone;
  LocOpe_SequenceOfPntFace myPoints;


};


#include <LocOpe_CurveShapeIntersector.lxx>





#endif // _LocOpe_CurveShapeIntersector_HeaderFile
