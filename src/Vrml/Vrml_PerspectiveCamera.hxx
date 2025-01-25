// Created on: 1997-02-12
// Created by: Alexander BRIVIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Vrml_PerspectiveCamera_HeaderFile
#define _Vrml_PerspectiveCamera_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Vec.hxx>
#include <Vrml_SFRotation.hxx>
#include <Standard_OStream.hxx>

//! specifies a PerspectiveCamera node of VRML specifying properties of cameras.
//! A perspective camera defines a perspective projection from a viewpoint. The viewing
//! volume for a perspective camera is a truncated right pyramid.
class Vrml_PerspectiveCamera
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_PerspectiveCamera();

  Standard_EXPORT Vrml_PerspectiveCamera(const gp_Vec&          aPosition,
                                         const Vrml_SFRotation& aOrientation,
                                         const Standard_Real    aFocalDistance,
                                         const Standard_Real    aHeightAngle);

  Standard_EXPORT void SetPosition(const gp_Vec& aPosition);

  Standard_EXPORT gp_Vec Position() const;

  Standard_EXPORT void SetOrientation(const Vrml_SFRotation& aOrientation);

  Standard_EXPORT Vrml_SFRotation Orientation() const;

  Standard_EXPORT void SetFocalDistance(const Standard_Real aFocalDistance);

  Standard_EXPORT Standard_Real FocalDistance() const;

  Standard_EXPORT void SetAngle(const Standard_Real aHeightAngle);

  Standard_EXPORT Standard_Real Angle() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

protected:
private:
  gp_Vec          myPosition;
  Vrml_SFRotation myOrientation;
  Standard_Real   myFocalDistance;
  Standard_Real   myHeightAngle;
};

#endif // _Vrml_PerspectiveCamera_HeaderFile
