// Created on: 1996-01-25
// Created by: Laurent PAINNOT
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

#ifndef _Extrema_GenLocateExtCS_HeaderFile
#define _Extrema_GenLocateExtCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
class Standard_DomainError;
class StdFail_NotDone;
class Adaptor3d_Curve;
class Adaptor3d_Surface;
class Extrema_POnCurv;
class Extrema_POnSurf;


//! With two close points it calculates the distance
//! between two surfaces.
//! This distance can be a minimum or a maximum.
class Extrema_GenLocateExtCS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_GenLocateExtCS();
  
  //! Calculates the distance with two close points.
  //! The close points are defined by the parameter values
  //! T for C and (U,V) for S.
  //! The function F(t,u,v)=distance(C(t),S(u,v))
  //! has an extremun when gradient(F)=0. The algorithm searchs
  //! a zero near the close points.
  Standard_EXPORT Extrema_GenLocateExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Real T, const Standard_Real U, const Standard_Real V, const Standard_Real Tol1, const Standard_Real Tol2);
  
  Standard_EXPORT void Perform (const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Real T, const Standard_Real U, const Standard_Real V, const Standard_Real Tol1, const Standard_Real Tol2);
  
  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the value of the extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance() const;
  
  //! Returns the point of the extremum distance on C.
  Standard_EXPORT const Extrema_POnCurv& PointOnCurve() const;
  
  //! Returns the point of the extremum distance on S.
  Standard_EXPORT const Extrema_POnSurf& PointOnSurface() const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Real mySqDist;
  Extrema_POnCurv myPoint1;
  Extrema_POnSurf myPoint2;


};







#endif // _Extrema_GenLocateExtCS_HeaderFile
