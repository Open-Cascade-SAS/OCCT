// Created on: 1996-12-23
// Created by: Alexander BRIVIN
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Vrml_SFRotation_HeaderFile
#define _Vrml_SFRotation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>

//! defines SFRotation type of VRML field types.
//! The  4  values  represent  an  axis  of  rotation  followed  by  amount  of
//! right-handed  rotation  about  the  that  axis, in  radians.
class Vrml_SFRotation
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_SFRotation();

  Standard_EXPORT Vrml_SFRotation(const Standard_Real aRotationX,
                                  const Standard_Real aRotationY,
                                  const Standard_Real aRotationZ,
                                  const Standard_Real anAngle);

  Standard_EXPORT void SetRotationX(const Standard_Real aRotationX);

  Standard_EXPORT Standard_Real RotationX() const;

  Standard_EXPORT void SetRotationY(const Standard_Real aRotationY);

  Standard_EXPORT Standard_Real RotationY() const;

  Standard_EXPORT void SetRotationZ(const Standard_Real aRotationZ);

  Standard_EXPORT Standard_Real RotationZ() const;

  Standard_EXPORT void SetAngle(const Standard_Real anAngle);

  Standard_EXPORT Standard_Real Angle() const;

protected:
private:
  Standard_Real myRotationX;
  Standard_Real myRotationY;
  Standard_Real myRotationZ;
  Standard_Real myAngle;
};

#endif // _Vrml_SFRotation_HeaderFile
