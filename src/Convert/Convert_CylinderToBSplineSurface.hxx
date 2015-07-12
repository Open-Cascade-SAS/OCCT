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

#ifndef _Convert_CylinderToBSplineSurface_HeaderFile
#define _Convert_CylinderToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
#include <Standard_Real.hxx>
class Standard_DomainError;
class gp_Cylinder;



//! This algorithm converts a bounded cylinder into a rational
//! B-spline surface. The cylinder is a Cylinder from package gp.
//! The parametrization of the cylinder is  :
//! P (U, V) = Loc + V * Zdir + Radius * (Xdir*Cos(U) + Ydir*Sin(U))
//! where Loc is the location point of the cylinder, Xdir, Ydir and
//! Zdir are the normalized directions of the local cartesian
//! coordinate system of the cylinder (Zdir is the direction of the
//! cylinder's axis). The U parametrization range is U [0, 2PI].
//! KeyWords :
//! Convert, Cylinder, BSplineSurface.
class Convert_CylinderToBSplineSurface  : public Convert_ElementarySurfaceToBSplineSurface
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The equivalent B-splineSurface as the same orientation as the
  //! cylinder in the U and V parametric directions.
  //!
  //! Raised if U1 = U2 or U1 = U2 + 2.0 * Pi
  //! Raised if V1 = V2.
  Standard_EXPORT Convert_CylinderToBSplineSurface(const gp_Cylinder& Cyl, const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  

  //! The equivalent B-splineSurface as the same orientation as the
  //! cylinder in the U and V parametric directions.
  //!
  //! Raised if V1 = V2.
  Standard_EXPORT Convert_CylinderToBSplineSurface(const gp_Cylinder& Cyl, const Standard_Real V1, const Standard_Real V2);




protected:





private:





};







#endif // _Convert_CylinderToBSplineSurface_HeaderFile
