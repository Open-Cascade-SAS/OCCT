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

#ifndef _Convert_ConeToBSplineSurface_HeaderFile
#define _Convert_ConeToBSplineSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
class gp_Cone;



//! This algorithm converts a bounded Cone into a rational
//! B-spline  surface.
//! The cone a Cone from package gp. Its parametrization is :
//! P (U, V) =  Loc + V * Zdir +
//! (R + V*Tan(Ang)) * (Cos(U)*Xdir + Sin(U)*Ydir)
//! where Loc is the location point of the cone, Xdir, Ydir and Zdir
//! are the normalized directions of the local cartesian coordinate
//! system of the cone (Zdir is the direction of the Cone's axis) ,
//! Ang is the cone semi-angle.  The U parametrization range is
//! [0, 2PI].
//! KeyWords :
//! Convert, Cone, BSplineSurface.
class Convert_ConeToBSplineSurface  : public Convert_ElementarySurfaceToBSplineSurface
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The equivalent B-spline surface as the same orientation as the
  //! Cone in the U and V parametric directions.
  //!
  //! Raised if U1 = U2 or U1 = U2 + 2.0 * Pi
  //! Raised if V1 = V2.
  Standard_EXPORT Convert_ConeToBSplineSurface(const gp_Cone& C, const Standard_Real U1, const Standard_Real U2, const Standard_Real V1, const Standard_Real V2);
  

  //! The equivalent B-spline surface as the same orientation as the
  //! Cone in the U and V parametric directions.
  //!
  //! Raised if V1 = V2.
  Standard_EXPORT Convert_ConeToBSplineSurface(const gp_Cone& C, const Standard_Real V1, const Standard_Real V2);




protected:





private:





};







#endif // _Convert_ConeToBSplineSurface_HeaderFile
