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

#ifndef _Aspect_XRPoseActionData_HeaderFile
#define _Aspect_XRPoseActionData_HeaderFile

#include <Aspect_TrackedDevicePose.hxx>
#include <Standard_TypeDef.hxx>

//! Pose input XR action data.
struct Aspect_XRPoseActionData
{
  Aspect_TrackedDevicePose Pose;         //!< pose state
  uint64_t                 ActiveOrigin; //!< The origin that caused this action's current state
                                         // clang-format off
  bool                     IsActive;     //!< whether or not this action is currently available to be bound in the active action set
                                         // clang-format on

  //! Empty constructor.
  Aspect_XRPoseActionData()
      : ActiveOrigin(0),
        IsActive(false)
  {
  }
};

#endif // _Aspect_XRPoseActionData_HeaderFile
