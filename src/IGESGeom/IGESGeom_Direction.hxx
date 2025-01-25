// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Kiran )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESGeom_Direction_HeaderFile
#define _IGESGeom_Direction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
class gp_Vec;

class IGESGeom_Direction;
DEFINE_STANDARD_HANDLE(IGESGeom_Direction, IGESData_IGESEntity)

//! defines IGESDirection, Type <123> Form <0>
//! in package IGESGeom
//! A direction entity is a non-zero vector in Euclidean 3-space
//! that is defined by its three components (direction ratios)
//! with respect to the coordinate axes. If x, y, z are the
//! direction ratios then (x^2 + y^2 + z^2) > 0
class IGESGeom_Direction : public IGESData_IGESEntity
{

public:
  Standard_EXPORT IGESGeom_Direction();

  //! This method is used to set the fields of the class
  //! Direction
  //! - aDirection : Direction ratios, Z is 0 by default
  Standard_EXPORT void Init(const gp_XYZ& aDirection);

  Standard_EXPORT gp_Vec Value() const;

  //! returns the Direction value after applying Transformation matrix
  Standard_EXPORT gp_Vec TransformedValue() const;

  DEFINE_STANDARD_RTTIEXT(IGESGeom_Direction, IGESData_IGESEntity)

protected:
private:
  gp_XYZ theDirection;
};

#endif // _IGESGeom_Direction_HeaderFile
