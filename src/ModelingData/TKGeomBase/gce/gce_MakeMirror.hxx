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

#ifndef _gce_MakeMirror_HeaderFile
#define _gce_MakeMirror_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
class gp_Pnt;
class gp_Ax1;
class gp_Lin;
class gp_Dir;
class gp_Pln;
class gp_Ax2;

//! This class implements elementary construction algorithms for a
//! symmetrical transformation in 3D space about a point,
//! axis or plane. The result is a gp_Trsf transformation.
//! A MakeMirror object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class gce_MakeMirror
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a central symmetry about a point.
  //! @param[in] Point center point
  Standard_EXPORT gce_MakeMirror(const gp_Pnt& Point);

  //! Constructs an axial symmetry about an axis.
  //! @param[in] Axis mirror axis
  Standard_EXPORT gce_MakeMirror(const gp_Ax1& Axis);

  //! Constructs an axial symmetry about a line.
  //! @param[in] Line mirror line
  Standard_EXPORT gce_MakeMirror(const gp_Lin& Line);

  //! Constructs an axial symmetry about an axis defined by point and direction.
  //! @param[in] Point point on the axis
  //! @param[in] Direc axis direction
  Standard_EXPORT gce_MakeMirror(const gp_Pnt& Point, const gp_Dir& Direc);

  //! Constructs a planar symmetry about a plane.
  //! @param[in] Plane mirror plane
  Standard_EXPORT gce_MakeMirror(const gp_Pln& Plane);

  //! Constructs a planar symmetry about a plane.
  //! @param[in] Plane mirror plane
  Standard_EXPORT gce_MakeMirror(const gp_Ax2& Plane);

  //! Returns the constructed transformation.
  //! @return resulting transformation
  Standard_EXPORT const gp_Trsf& Value() const;

  //! Alias for Value().
  //! @return resulting transformation
  const gp_Trsf& Operator() const
  {
    return Value();
  }

  //! Conversion operator returning the constructed transformation.
  //! @return resulting transformation
  operator const gp_Trsf&() const
  {
    return Operator();
  }

private:
  gp_Trsf TheMirror;
};

#endif // _gce_MakeMirror_HeaderFile
