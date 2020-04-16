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

#ifndef _Aspect_TrackedDevicePose_HeaderFile
#define _Aspect_TrackedDevicePose_HeaderFile

#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>

//! Describes a single pose for a tracked object (for XR).
struct Aspect_TrackedDevicePose
{
  gp_Trsf Orientation;       //!< device to absolute transformation
  gp_Vec  Velocity;          //!< velocity in tracker space in m/s
  gp_Vec  AngularVelocity;   //!< angular velocity in radians/s
  bool    IsValidPose;       //!< indicates valid pose
  bool    IsConnectedDevice; //!< indicates connected state

  //! Empty constructor.
  Aspect_TrackedDevicePose() : IsValidPose (false), IsConnectedDevice (false) {}
};

//! Array of tracked poses.
typedef NCollection_Array1<Aspect_TrackedDevicePose> Aspect_TrackedDevicePoseArray;

#endif // _Aspect_TrackedDevicePose_HeaderFile
