// Created on: 2014-12-30
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_ZLayerId_HeaderFile
#define _Graphic3d_ZLayerId_HeaderFile

#include <Standard_Integer.hxx>

typedef Standard_Integer Graphic3d_ZLayerId;

//! This enumeration defines the list of predefined layers, which can not be removed (but settings
//! can be overridden). Custom layers might be added with positive index (>= 1) if standard list is
//! insufficient for application needs; these layers will be displayed on top of predefined ones.
enum
{
  Graphic3d_ZLayerId_UNKNOWN = -1, //!< identifier for invalid ZLayer
  Graphic3d_ZLayerId_Default = 0,  //!< default Z-layer for main presentations
  Graphic3d_ZLayerId_Top =
    -2, //!< overlay for 3D presentations which inherits Depth from previous ZLayer
  Graphic3d_ZLayerId_Topmost = -3, //!< overlay for 3D presentations with independent Depth
  Graphic3d_ZLayerId_TopOSD  = -4, //!< overlay for 2D presentations (On-Screen-Display)
  Graphic3d_ZLayerId_BotOSD  = -5  //!< underlay for 2D presentations (On-Screen-Display)
};

#endif // _Graphic3d_ZLayerId_HeaderFile
