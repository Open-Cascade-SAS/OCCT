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

#ifndef _Plate_D2_HeaderFile
#define _Plate_D2_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>

//! define an order 2 derivatives of a 3d valued
//! function of a 2d variable
class Plate_D2
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Plate_D2(const gp_XYZ& duu, const gp_XYZ& duv, const gp_XYZ& dvv);

  Standard_EXPORT Plate_D2(const Plate_D2& ref);

  friend class Plate_GtoCConstraint;
  friend class Plate_FreeGtoCConstraint;

protected:
private:
  gp_XYZ Duu;
  gp_XYZ Duv;
  gp_XYZ Dvv;
};

#endif // _Plate_D2_HeaderFile
