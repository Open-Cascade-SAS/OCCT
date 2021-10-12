// Created on: 1991-02-21
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Extrema_ExtPElC_HeaderFile
#define _Extrema_ExtPElC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_POnCurv.hxx>
class gp_Pnt;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Hypr;
class gp_Parab;


//! It calculates all the distances between a point
//! and an elementary curve.
//! These distances can be minimum or maximum.
class Extrema_ExtPElC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtPElC();
  
  //! Calculates the extremum distance between the
  //! point P and the segment [Uinf,Usup] of the line C.
  Standard_EXPORT Extrema_ExtPElC(const gp_Pnt& P, const gp_Lin& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Lin& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! Calculates the 2 extremum distances between the
  //! point P and the segment [Uinf,Usup] of the circle C.
  //! Tol is used to determine
  //! if P is on the axis of the circle or
  //! if an extremum is on an endpoint of the segment.
  //! If P is on the axis of the circle,
  //! there are infinite solution then IsDone(me)=False.
  //! The conditions on the Uinf and Usup are:
  //! 0. <= Uinf <= 2.*PI and Usup > Uinf.
  //! If Usup > Uinf + 2.*PI, then only the solutions in
  //! the range [Uinf,Uinf+2.*PI[ are computed.
  Standard_EXPORT Extrema_ExtPElC(const gp_Pnt& P, const gp_Circ& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Circ& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! Calculates the 4 extremum distances between the
  //! point P and the segment [Uinf,Usup] of the ellipse C.
  //! Tol is used to determine
  //! if the point is on the axis of the ellipse and
  //! if the major radius is equal to the minor radius or
  //! if an extremum is on an endpoint of the segment.
  //! If P is on the axis of the ellipse,
  //! there are infinite solution then IsDone(me)=False.
  //! The conditions on the Uinf and Usup are:
  //! 0. <= Uinf <= 2.*PI and Usup > Uinf.
  //! If Usup > Uinf + 2.*PI, then only the solutions in
  //! the range [Uinf,Uinf+2.*PI[ are computed.
  Standard_EXPORT Extrema_ExtPElC(const gp_Pnt& P, const gp_Elips& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Elips& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! Calculates the extremum distances between the
  //! point P and the segment [Uinf,Usup] of the hyperbola
  //! C.
  //! Tol is used to determine if two solutions u and v
  //! are identical; the condition is:
  //! dist(C(u),C(v)) < Tol.
  Standard_EXPORT Extrema_ExtPElC(const gp_Pnt& P, const gp_Hypr& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Hypr& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! Calculates the 4 extremum distances between the
  //! point P and the segment [Uinf,Usup] of the parabola
  //! C.
  //! Tol is used to determine if two solutions u and v
  //! are identical; the condition is:
  //! dist(C(u),C(v)) < Tol.
  Standard_EXPORT Extrema_ExtPElC(const gp_Pnt& P, const gp_Parab& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Parab& C, const Standard_Real Tol, const Standard_Real Uinf, const Standard_Real Usup);
  
  //! True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns True if the Nth extremum distance is a
  //! minimum.
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  //! Returns the point of the Nth extremum distance.
  Standard_EXPORT const Extrema_POnCurv& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Integer myNbExt;
  Standard_Real mySqDist[4];
  Standard_Boolean myIsMin[4];
  Extrema_POnCurv myPoint[4];


};







#endif // _Extrema_ExtPElC_HeaderFile
