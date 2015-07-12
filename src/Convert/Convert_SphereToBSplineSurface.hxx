// Created on: 1991-10-10
// Created by: Jean Claude VAUTHIER
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

#ifndef _Convert_SphereToBSplineSurface_HeaderFile
#define _Convert_SphereToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Standard_DomainError;
class gp_Sphere;



//! This algorithm converts a bounded Sphere into a rational
//! B-spline surface. The sphere is a Sphere from package gp.
//! The parametrization of the sphere is
//! P (U, V) = Loc  + Radius * Sin(V) * Zdir +
//! Radius * Cos(V) * (Cos(U)*Xdir + Sin(U)*Ydir)
//! where Loc is the center of the sphere Xdir, Ydir and Zdir are the
//! normalized directions of the local cartesian coordinate system of
//! the sphere. The parametrization range is U [0, 2PI] and
//! V [-PI/2, PI/2].
//! KeyWords :
//! Convert, Sphere, BSplineSurface.
class Convert_SphereToBSplineSurface  : public Convert_ElementarySurfaceToBSplineSurface
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The equivalent B-spline surface as the same orientation as the
  //! sphere in the U and V parametric directions.
  //!
  //! Raised if U1 = U2 or U1 = U2 + 2.0 * Pi
  //! Raised if V1 = V2.
  Standard_EXPORT Convert_SphereToBSplineSurface(const gp_Sphere& Sph, const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  

  //! The equivalent B-spline surface as the same orientation
  //! as the sphere in the U and V parametric directions.
  //!
  //! Raised if UTrim = True and Param1 = Param2 or
  //! Param1 = Param2 + 2.0 * Pi
  //! Raised if UTrim = False and Param1 = Param2
  Standard_EXPORT Convert_SphereToBSplineSurface(const gp_Sphere& Sph, const Standard_Real Param1, const Standard_Real Param2, const Standard_Boolean UTrim = Standard_True);
  

  //! The equivalent B-spline surface as the same orientation
  //! as the sphere in the U and V parametric directions.
  Standard_EXPORT Convert_SphereToBSplineSurface(const gp_Sphere& Sph);




protected:





private:





};







#endif // _Convert_SphereToBSplineSurface_HeaderFile
