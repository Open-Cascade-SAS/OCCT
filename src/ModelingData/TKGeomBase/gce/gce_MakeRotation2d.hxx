// Created on: 1992-08-26
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

#ifndef _gce_MakeRotation2d_HeaderFile
#define _gce_MakeRotation2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf2d.hxx>
class gp_Pnt2d;

//! Implements an elementary construction algorithm for
//! a rotation in 2D space. The result is a gp_Trsf2d transformation.
//! A MakeRotation2d object provides a framework for:
//! -   defining the construction of the transformation,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class gce_MakeRotation2d
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a rotation around a point in 2D.
  //! @param[in] Point rotation center
  //! @param[in] Angle rotation angle in radians
  Standard_EXPORT gce_MakeRotation2d(const gp_Pnt2d& Point, const double Angle);

  //! Returns the constructed transformation.
  //! @return resulting transformation
  Standard_EXPORT const gp_Trsf2d& Value() const;

  //! Alias for Value().
  //! @return resulting transformation
  const gp_Trsf2d& Operator() const
  {
    return Value();
  }

  //! Conversion operator returning the constructed transformation.
  //! @return resulting transformation
  operator const gp_Trsf2d&() const
  {
    return Operator();
  }

private:
  gp_Trsf2d TheRotation2d;
};

#endif // _gce_MakeRotation2d_HeaderFile
