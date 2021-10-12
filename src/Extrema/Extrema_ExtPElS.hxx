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

#ifndef _Extrema_ExtPElS_HeaderFile
#define _Extrema_ExtPElS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_POnSurf.hxx>
class gp_Pnt;
class gp_Cylinder;
class gp_Pln;
class gp_Cone;
class gp_Torus;
class gp_Sphere;


//! It calculates all the extremum distances
//! between a point and a surface.
//! These distances can be minimum or maximum.
class Extrema_ExtPElS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtPElS();
  
  //! It calculates all the distances between a point
  //! and a cylinder from gp.
  //! Tol is used to test if the point is on the axis.
  Standard_EXPORT Extrema_ExtPElS(const gp_Pnt& P, const gp_Cylinder& S, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Cylinder& S, const Standard_Real Tol);
  
  //! It calculates all the distances between a point
  //! and a plane from gp.
  //! Tol is used to test if the point is on the plane.
  Standard_EXPORT Extrema_ExtPElS(const gp_Pnt& P, const gp_Pln& S, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Pln& S, const Standard_Real Tol);
  
  //! It calculates all the distances between a point
  //! and a cone from gp.
  //! Tol is used to test if the point is at the apex or
  //! on the axis.
  Standard_EXPORT Extrema_ExtPElS(const gp_Pnt& P, const gp_Cone& S, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Cone& S, const Standard_Real Tol);
  
  //! It calculates all the distances between a point
  //! and a torus from gp.
  //! Tol is used to test if the point is on the axis.
  Standard_EXPORT Extrema_ExtPElS(const gp_Pnt& P, const gp_Torus& S, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Torus& S, const Standard_Real Tol);
  
  //! It calculates all the distances between a point
  //! and a sphere from gp.
  //! Tol is used to test if the point is at the center.
  Standard_EXPORT Extrema_ExtPElS(const gp_Pnt& P, const gp_Sphere& S, const Standard_Real Tol);
  
  Standard_EXPORT void Perform (const gp_Pnt& P, const gp_Sphere& S, const Standard_Real Tol);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth resulting square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the point of the Nth resulting distance.
  Standard_EXPORT const Extrema_POnSurf& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Integer myNbExt;
  Standard_Real mySqDist[4];
  Extrema_POnSurf myPoint[4];


};







#endif // _Extrema_ExtPElS_HeaderFile
