// Created on: 1992-11-05
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepPrim_Torus_HeaderFile
#define _BRepPrim_Torus_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepPrim_Revolution.hxx>
class gp_Ax2;
class gp_Pnt;
class TopoDS_Face;

//! Implements the torus primitive
class BRepPrim_Torus : public BRepPrim_Revolution
{
public:
  DEFINE_STANDARD_ALLOC

  //! the STEP definition
  //! Position : center and axes
  //! Major, Minor : Radii
  //!
  //! Errors : Major < Resolution
  //! Minor < Resolution
  Standard_EXPORT BRepPrim_Torus(const gp_Ax2&       Position,
                                 const Standard_Real Major,
                                 const Standard_Real Minor);

  //! Torus centered at origin
  Standard_EXPORT BRepPrim_Torus(const Standard_Real Major, const Standard_Real Minor);

  //! Torus at Center
  Standard_EXPORT BRepPrim_Torus(const gp_Pnt&       Center,
                                 const Standard_Real Major,
                                 const Standard_Real Minor);

  //! The surface normal should be directed  towards the
  //! outside.
  Standard_EXPORT virtual TopoDS_Face MakeEmptyLateralFace() const Standard_OVERRIDE;

protected:
private:
  Standard_EXPORT void SetMeridian();

  Standard_Real myMajor; //!< distance from the center of the pipe to the center of the torus
  Standard_Real myMinor; //!< radius of the pipe
};

#endif // _BRepPrim_Torus_HeaderFile
