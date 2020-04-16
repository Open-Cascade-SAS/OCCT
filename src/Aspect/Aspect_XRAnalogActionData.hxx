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

#ifndef _Aspect_XRAnalogActionData_HeaderFile
#define _Aspect_XRAnalogActionData_HeaderFile

#include <NCollection_Vec3.hxx>

//! Analog input XR action data.
struct Aspect_XRAnalogActionData
{
  uint64_t                ActiveOrigin; //!< The origin that caused this action's current state
  float                   UpdateTime;   //!< Time relative to now when this event happened. Will be negative to indicate a past time
  NCollection_Vec3<float> VecXYZ;       //!< the current state of this action
  NCollection_Vec3<float> DeltaXYZ;     //!< deltas since the previous update
  bool                    IsActive;     //!< whether or not this action is currently available to be bound in the active action set

  //! Return TRUE if delta is non-zero.
  bool IsChanged() { return !DeltaXYZ.IsEqual (NCollection_Vec3<float> (0.0f, 0.0f, 0.0f)); }

  //! Empty constructor.
  Aspect_XRAnalogActionData() : ActiveOrigin (0), UpdateTime (0.0f), IsActive (false) {}
};

#endif // _Aspect_XRAnalogActionData_HeaderFile
