// Created on: 1993-03-17
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

#ifndef _ApproxInt_SvSurfaces_HeaderFile
#define _ApproxInt_SvSurfaces_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class gp_Pnt;
class gp_Vec;
class gp_Vec2d;
class IntSurf_PntOn2S;

//! This class is root class for classes dedicated to calculate 
//! 2d and 3d points and tangents of intersection lines of two surfaces of different types
//! for given u, v parameters of intersection point on two surfaces.
//! 
//! The field myUseSolver is used to manage type of calculation:
//! if myUseSolver = true, input parameters u1, v1, u2, v2 are considered as first approximation of 
//! exact intersection point, then coordinates u1, v1, u2, v2 are refined with help of 
//! the solver used in intersection algorithm and required values are calculated.
//! if myUseSolver = false, u1, v1, u2, v2 are considered as "exact" intersection points on two surfaces
//! and required values are calculated directly using u1, v1, u2, v2
class ApproxInt_SvSurfaces
{
public:

  DEFINE_STANDARD_ALLOC

  ApproxInt_SvSurfaces() : myUseSolver (false) {}

  //! returns True if Tg,Tguv1 Tguv2 can be computed.
  Standard_EXPORT virtual Standard_Boolean Compute (Standard_Real& u1, Standard_Real& v1,
                                                    Standard_Real& u2, Standard_Real& v2,
                                                    gp_Pnt& Pt,
                                                    gp_Vec& Tg,
                                                    gp_Vec2d& Tguv1,
                                                    gp_Vec2d& Tguv2) = 0;
  
  Standard_EXPORT virtual void Pnt (const Standard_Real u1, const Standard_Real v1,
                                    const Standard_Real u2, const Standard_Real v2,
                                    gp_Pnt& P) = 0;

  //! computes point on curve and parameters on the surfaces
  Standard_EXPORT virtual Standard_Boolean SeekPoint(const Standard_Real u1, const Standard_Real v1,
                                                     const Standard_Real u2, const Standard_Real v2,
                                                     IntSurf_PntOn2S& Point) = 0;
  
  Standard_EXPORT virtual Standard_Boolean Tangency (const Standard_Real u1, const Standard_Real v1,
                                                     const Standard_Real u2, const Standard_Real v2,
                                                     gp_Vec& Tg) = 0;
  
  Standard_EXPORT virtual Standard_Boolean TangencyOnSurf1 (const Standard_Real u1,
                                                            const Standard_Real v1,
                                                            const Standard_Real u2,
                                                            const Standard_Real v2,
                                                            gp_Vec2d& Tg) = 0;
  
  Standard_EXPORT virtual Standard_Boolean TangencyOnSurf2 (const Standard_Real u1,
                                                            const Standard_Real v1,
                                                            const Standard_Real u2,
                                                            const Standard_Real v2,
                                                            gp_Vec2d& Tg) = 0;
  Standard_EXPORT virtual ~ApproxInt_SvSurfaces();

  void SetUseSolver (const Standard_Boolean theUseSol)
  {
    myUseSolver = theUseSol;
  }

  virtual Standard_Boolean GetUseSolver() const
  {
    return myUseSolver;
  }

private:

  Standard_Boolean myUseSolver;

};

#endif // _ApproxInt_SvSurfaces_HeaderFile
