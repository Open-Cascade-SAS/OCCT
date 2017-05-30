// Created on: 1992-11-13
// Created by: GG
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

#ifndef _V3d_HeaderFile
#define _V3d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <V3d_TypeOfOrientation.hxx>
#include <Standard_Real.hxx>

class V3d_View;

//! This package contains the set of commands and services
//! of the 3D Viewer. It provides a set of high level commands
//! to control the views and viewing modes.
class V3d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Determines the orientation vector corresponding
  //! to the predefined orientation type.
  Standard_EXPORT static Graphic3d_Vector GetProjAxis (const V3d_TypeOfOrientation Orientation);

  //! Compute the graphic structure of arrow.
  //! X0,Y0,Z0 : coordinate of the arrow.
  //! DX,DY,DZ : Direction of the arrow.
  //! Alpha    : Angle of arrow.
  //! Lng      : Length of arrow.
  Standard_EXPORT static void ArrowOfRadius (const Handle(Graphic3d_Group)& garrow,
                                             const Standard_Real X0, const Standard_Real Y0, const Standard_Real Z0,
                                             const Standard_Real DX, const Standard_Real DY, const Standard_Real DZ,
                                             const Standard_Real Alpha,
                                             const Standard_Real Lng);

  //! Compute the graphic structure of circle.
  //! X0,Y0,Z0 : Center of circle.
  //! VX,VY,VZ : Axis of circle.
  //! Radius   : Radius of circle.
  Standard_EXPORT static void CircleInPlane (const Handle(Graphic3d_Group)& gcircle,
                                             const Standard_Real X0, const Standard_Real Y0, const Standard_Real Z0,
                                             const Standard_Real VX, const Standard_Real VY, const Standard_Real VZ,
                                             const Standard_Real Radius);

  Standard_EXPORT static void SwitchViewsinWindow (const Handle(V3d_View)& aPreviousView, const Handle(V3d_View)& aNextView);

};

#endif // _V3d_HeaderFile
