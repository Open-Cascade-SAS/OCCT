// Created on: 1997-11-06
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _ProjLib_PrjResolve_HeaderFile
#define _ProjLib_PrjResolve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt2d.hxx>

class Adaptor3d_Curve;
class Adaptor3d_Surface;


class ProjLib_PrjResolve 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ProjLib_PrjResolve(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S, const Standard_Integer Fix);
  
  //! Calculates the ort from  C(t)  to  S  with a close point.
  //! The close point is defined by the parameter values U0 and V0.
  //! The function F(u,v)=distance(S(u,v),C(t)) has an extremum when gradient(F)=0.
  //! The algorithm searches a zero near the close point.
  Standard_EXPORT void Perform (const Standard_Real t, const Standard_Real U, const Standard_Real V, const gp_Pnt2d& Tol, const gp_Pnt2d& Inf, const gp_Pnt2d& Sup, const Standard_Real FTol = -1, const Standard_Boolean StrictInside = Standard_False);
  
  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the point of the extremum distance.
  Standard_EXPORT gp_Pnt2d Solution() const;

private:

  const Adaptor3d_Curve* myCurve;
  const Adaptor3d_Surface* mySurface;
  Standard_Boolean myDone;
  gp_Pnt2d mySolution;
  Standard_Integer myFix;

};

#endif // _ProjLib_PrjResolve_HeaderFile
