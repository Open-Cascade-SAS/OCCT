// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _AIS_RotationMode_HeaderFile
#define _AIS_RotationMode_HeaderFile

//! Camera rotation mode.
enum AIS_RotationMode
{
  AIS_RotationMode_BndBoxActive, //!< default OCCT rotation
  AIS_RotationMode_PickLast,     //!< rotate around last picked point
  AIS_RotationMode_PickCenter,   //!< rotate around point at the center of window
  AIS_RotationMode_CameraAt,     //!< rotate around camera center
  AIS_RotationMode_BndBoxScene,  //!< rotate around scene center
};

enum
{
  AIS_RotationMode_LOWER = 0,
  AIS_RotationMode_UPPER = AIS_RotationMode_BndBoxScene,
};

#endif // _AIS_RotationMode_HeaderFile
