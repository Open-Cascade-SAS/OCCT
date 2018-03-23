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

#include <gp_Dir.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Real.hxx>
#include <V3d_TypeOfOrientation.hxx>

class Graphic3d_Group;
class V3d_View;

//! This package contains the set of commands and services
//! of the 3D Viewer. It provides a set of high level commands
//! to control the views and viewing modes.
class V3d 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Determines the orientation vector corresponding to the predefined orientation type.
  static gp_Dir GetProjAxis (const V3d_TypeOfOrientation theOrientation)
  {
    switch (theOrientation)
    {
      case V3d_Xpos:         return  gp::DX();
      case V3d_Ypos:         return  gp::DY();
      case V3d_Zpos:         return  gp::DZ();
      case V3d_Xneg:         return -gp::DX();
      case V3d_Yneg:         return -gp::DY();
      case V3d_Zneg:         return -gp::DZ();
      case V3d_XposYposZpos: return gp_Dir ( 1,  1,  1);
      case V3d_XposYposZneg: return gp_Dir ( 1,  1, -1);
      case V3d_XposYnegZpos: return gp_Dir ( 1, -1,  1);
      case V3d_XposYnegZneg: return gp_Dir ( 1, -1, -1);
      case V3d_XnegYposZpos: return gp_Dir (-1,  1,  1);
      case V3d_XnegYposZneg: return gp_Dir (-1,  1, -1);
      case V3d_XnegYnegZpos: return gp_Dir (-1, -1,  1);
      case V3d_XnegYnegZneg: return gp_Dir (-1, -1, -1);
      case V3d_XposYpos:     return gp_Dir ( 1,  1,  0);
      case V3d_XposYneg:     return gp_Dir ( 1, -1,  0);
      case V3d_XnegYpos:     return gp_Dir (-1,  1,  0);
      case V3d_XnegYneg:     return gp_Dir (-1, -1,  0);
      case V3d_XposZpos:     return gp_Dir ( 1,  0,  1);
      case V3d_XposZneg:     return gp_Dir ( 1,  0, -1);
      case V3d_XnegZpos:     return gp_Dir (-1,  0,  1);
      case V3d_XnegZneg:     return gp_Dir (-1,  0, -1);
      case V3d_YposZpos:     return gp_Dir ( 0,  1,  1);
      case V3d_YposZneg:     return gp_Dir ( 0,  1, -1);
      case V3d_YnegZpos:     return gp_Dir ( 0, -1,  1);
      case V3d_YnegZneg:     return gp_Dir ( 0, -1, -1);
    }
    return gp_Dir (0, 0, 0);
  }

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

  //! Returns the string name for a given orientation type.
  //! @param theType orientation type
  //! @return string identifier from the list Xpos, Ypos, Zpos and others
  Standard_EXPORT static Standard_CString TypeOfOrientationToString (V3d_TypeOfOrientation theType);

  //! Returns the orientation type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @return orientation type or V3d_TypeOfOrientation if string identifier is invalid
  static V3d_TypeOfOrientation TypeOfOrientationFromString (Standard_CString theTypeString)
  {
    V3d_TypeOfOrientation aType = V3d_Xpos;
    TypeOfOrientationFromString (theTypeString, aType);
    return aType;
  }

  //! Determines the shape type from the given string identifier (using case-insensitive comparison).
  //! @param theTypeString string identifier
  //! @param theType detected shape type
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean TypeOfOrientationFromString (const Standard_CString theTypeString,
                                                                       V3d_TypeOfOrientation& theType);

};

#endif // _V3d_HeaderFile
