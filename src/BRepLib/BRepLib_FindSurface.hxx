// Created on: 1994-07-22
// Created by: Remi LEQUETTE
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

#ifndef _BRepLib_FindSurface_HeaderFile
#define _BRepLib_FindSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopLoc_Location.hxx>
class Geom_Surface;
class Standard_NoSuchObject;
class TopoDS_Shape;
class TopLoc_Location;


//! Provides an  algorithm to find  a Surface  through a
//! set of edges.
//!
//! The edges  of  the  shape  given  as  argument are
//! explored if they are not coplanar at  the required
//! tolerance  the method Found returns false.
//!
//! If a null tolerance is given the max of the  edges
//! tolerances is used.
//!
//! The method Tolerance returns the true distance  of
//! the edges to the Surface.
//!
//! The method Surface returns the Surface if found.
//!
//! The method Existed  returns returns  True  if  the
//! Surface was already attached to some of the edges.
//!
//! When Existed  returns True  the  Surface  may have a
//! location given by the Location method.
class BRepLib_FindSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepLib_FindSurface();
  
  //! Computes the Surface from the edges of  <S> with the
  //! given tolerance.
  //! if <OnlyPlane> is true, the computed surface will be
  //! a plane. If it is not possible to find a plane, the
  //! flag NotDone will be set.
  //! If <OnlyClosed> is true,  then  S  sould be a wire
  //! and the existing surface,  on  which wire S is not
  //! closed in 2D, will be ignored.
  Standard_EXPORT BRepLib_FindSurface(const TopoDS_Shape& S, const Standard_Real Tol = -1, const Standard_Boolean OnlyPlane = Standard_False, const Standard_Boolean OnlyClosed = Standard_False);
  
  //! Computes the Surface from the edges of  <S> with the
  //! given tolerance.
  //! if <OnlyPlane> is true, the computed surface will be
  //! a plane. If it is not possible to find a plane, the
  //! flag NotDone will be set.
  //! If <OnlyClosed> is true,  then  S  sould be a wire
  //! and the existing surface,  on  which wire S is not
  //! closed in 2D, will be ignored.
  Standard_EXPORT void Init (const TopoDS_Shape& S, const Standard_Real Tol = -1, const Standard_Boolean OnlyPlane = Standard_False, const Standard_Boolean OnlyClosed = Standard_False);
  
  Standard_EXPORT Standard_Boolean Found() const;
  
  Standard_EXPORT Handle(Geom_Surface) Surface() const;
  
  Standard_EXPORT Standard_Real Tolerance() const;
  
  Standard_EXPORT Standard_Real ToleranceReached() const;
  
  Standard_EXPORT Standard_Boolean Existed() const;
  
  Standard_EXPORT TopLoc_Location Location() const;




protected:





private:



  Handle(Geom_Surface) mySurface;
  Standard_Real myTolerance;
  Standard_Real myTolReached;
  Standard_Boolean isExisted;
  TopLoc_Location myLocation;


};







#endif // _BRepLib_FindSurface_HeaderFile
