// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GCE2d_MakeMirror_HeaderFile
#define _GCE2d_MakeMirror_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom2d_Transformation;
class gp_Pnt2d;
class gp_Ax2d;
class gp_Lin2d;
class gp_Dir2d;

//! This class implements elementary construction algorithms for a
//! symmetrical transformation in 2D space about a point
//! or axis. The result is a Geom2d_Transformation transformation.
//! A MakeMirror object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GCE2d_MakeMirror
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GCE2d_MakeMirror(const gp_Pnt2d& Point);

  Standard_EXPORT GCE2d_MakeMirror(const gp_Ax2d& Axis);

  Standard_EXPORT GCE2d_MakeMirror(const gp_Lin2d& Line);

  //! Make a symmetry transformation af axis defined by
  //! <Point> and <Direc>.
  Standard_EXPORT GCE2d_MakeMirror(const gp_Pnt2d& Point, const gp_Dir2d& Direc);

  //! Returns the constructed transformation.
  Standard_EXPORT const Handle(Geom2d_Transformation)& Value() const;

  operator const Handle(Geom2d_Transformation) & () const { return Value(); }

protected:
private:
  Handle(Geom2d_Transformation) TheMirror;
};

#endif // _GCE2d_MakeMirror_HeaderFile
