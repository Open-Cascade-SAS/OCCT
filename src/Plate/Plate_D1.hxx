// Created on: 1995-10-19
// Created by: Andre LIEUTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Plate_D1_HeaderFile
#define _Plate_D1_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>

//! define an order 1 derivatives of a 3d valued
//! function of a 2d variable
class Plate_D1
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Plate_D1(const gp_XYZ& du, const gp_XYZ& dv);

  Standard_EXPORT Plate_D1(const Plate_D1& ref);

  const gp_XYZ& DU() const;

  const gp_XYZ& DV() const;

  friend class Plate_GtoCConstraint;
  friend class Plate_FreeGtoCConstraint;

protected:
private:
  gp_XYZ Du;
  gp_XYZ Dv;
};

#include <Plate_D1.lxx>

#endif // _Plate_D1_HeaderFile
