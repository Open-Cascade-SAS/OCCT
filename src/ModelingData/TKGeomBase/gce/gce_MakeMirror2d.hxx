// Created on: 1992-09-01
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

#ifndef _gce_MakeMirror2d_HeaderFile
#define _gce_MakeMirror2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf2d.hxx>
class gp_Pnt2d;
class gp_Ax2d;
class gp_Lin2d;
class gp_Dir2d;

//! This class implements elementary construction algorithms for a
//! symmetrical transformation in 2D space about a point
//! or axis. The result is a gp_Trsf2d transformation.
//! A MakeMirror2d object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and consulting the result.
class gce_MakeMirror2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a central symmetry about a point.
  //! @param[in] Point center point
  Standard_EXPORT gce_MakeMirror2d(const gp_Pnt2d& Point);

  //! Constructs an axial symmetry about an axis.
  //! @param[in] Axis mirror axis
  Standard_EXPORT gce_MakeMirror2d(const gp_Ax2d& Axis);

  //! Constructs an axial symmetry about a line.
  //! @param[in] Line mirror line
  Standard_EXPORT gce_MakeMirror2d(const gp_Lin2d& Line);

  //! Constructs an axial symmetry about an axis defined by point and direction.
  //! @param[in] Point point on the axis
  //! @param[in] Direc axis direction
  Standard_EXPORT gce_MakeMirror2d(const gp_Pnt2d& Point, const gp_Dir2d& Direc);

  //! Returns the constructed transformation.
  //! @return resulting transformation
  Standard_EXPORT const gp_Trsf2d& Value() const;

  //! Alias for Value() returning a copy.
  //! @return resulting transformation
  gp_Trsf2d Operator() const
  {
    return Value();
  }

  //! Conversion operator returning the constructed transformation.
  //! @return resulting transformation
  operator gp_Trsf2d() const
  {
    return Operator();
  }

private:
  gp_Trsf2d TheMirror2d;
};

#endif // _gce_MakeMirror2d_HeaderFile
