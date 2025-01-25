// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _Aspect_XRTrackedDeviceRole_HeaderFile
#define _Aspect_XRTrackedDeviceRole_HeaderFile

//! Predefined tracked devices.
enum Aspect_XRTrackedDeviceRole
{
  Aspect_XRTrackedDeviceRole_Head,      //!< head
  Aspect_XRTrackedDeviceRole_LeftHand,  //!< left hand
  Aspect_XRTrackedDeviceRole_RightHand, //!< right hand
  Aspect_XRTrackedDeviceRole_Other,     //!< other devices
};

enum
{
  Aspect_XRTrackedDeviceRole_NB = Aspect_XRTrackedDeviceRole_Other + 1
};

#endif // _Aspect_XRTrackedDeviceRole_HeaderFile
