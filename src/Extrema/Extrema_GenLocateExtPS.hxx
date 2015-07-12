// Created on: 1995-07-18
// Created by: Modelistation
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

#ifndef _Extrema_GenLocateExtPS_HeaderFile
#define _Extrema_GenLocateExtPS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Extrema_POnSurf.hxx>
class Standard_DomainError;
class StdFail_NotDone;
class gp_Pnt;
class Adaptor3d_Surface;
class Extrema_POnSurf;


//! With a close point, it calculates the distance
//! between a point and a surface.
//! This distance can be a minimum or a maximum.
class Extrema_GenLocateExtPS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_GenLocateExtPS();
  
  //! Calculates the distance with a close point.
  //! The close point is defined by the parameter values
  //! U0 and V0.
  //! The function F(u,v)=distance(S(u,v),p) has an
  //! extremun when gradient(F)=0. The algorithm searchs
  //! a zero near the close point.
  Standard_EXPORT Extrema_GenLocateExtPS(const gp_Pnt& P, const Adaptor3d_Surface& S, const Standard_Real U0, const Standard_Real V0, const Standard_Real TolU, const Standard_Real TolV);
  
  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the value of the extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance() const;
  
  //! Returns the point of the extremum distance.
  Standard_EXPORT const Extrema_POnSurf& Point() const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Real mySqDist;
  Extrema_POnSurf myPoint;


};







#endif // _Extrema_GenLocateExtPS_HeaderFile
