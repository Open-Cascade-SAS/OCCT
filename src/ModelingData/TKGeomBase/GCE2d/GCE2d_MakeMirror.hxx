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

//! This class implements elementary construction algorithms for
//! symmetric transformations in 2D space about a point, axis, or line.
//! The result is a `Geom2d_Transformation`.
//! A `GCE2d_MakeMirror` object provides a framework for:
//! - defining the transformation parameters;
//! - running the construction algorithm;
//! - querying the resulting transformation via `Value()`.
class GCE2d_MakeMirror
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a central symmetry about a point.
  //! @param[in] thePoint center point
  Standard_EXPORT GCE2d_MakeMirror(const gp_Pnt2d& thePoint);

  //! Constructs an axial symmetry about an axis.
  //! @param[in] theAxis symmetry axis
  Standard_EXPORT GCE2d_MakeMirror(const gp_Ax2d& theAxis);

  //! Constructs an axial symmetry about a line.
  //! @param[in] theLine symmetry line
  Standard_EXPORT GCE2d_MakeMirror(const gp_Lin2d& theLine);

  //! Constructs an axial symmetry about a line defined by point and direction.
  //! @param[in] thePoint point on symmetry axis
  //! @param[in] theDirec symmetry direction
  Standard_EXPORT GCE2d_MakeMirror(const gp_Pnt2d& thePoint, const gp_Dir2d& theDirec);

  //! Returns the constructed transformation.
  //! @return resulting transformation
  Standard_EXPORT const occ::handle<Geom2d_Transformation>& Value() const;

  //! Conversion operator returning the constructed object.
  //! @return resulting transformation
  operator const occ::handle<Geom2d_Transformation>&() const { return Value(); }

private:
  occ::handle<Geom2d_Transformation> TheMirror;
};

#endif // _GCE2d_MakeMirror_HeaderFile
