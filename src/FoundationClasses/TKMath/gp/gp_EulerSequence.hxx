// Created on: 1993-04-13
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _gp_EulerSequence_HeaderFile
#define _gp_EulerSequence_HeaderFile

//! Enumerates all 24 possible variants of generalized
//! Euler angles, defining general 3d rotation by three
//! rotations around main axes of coordinate system,
//! in different possible orders.
//!
//! The name of the enumeration
//! corresponds to order of rotations, prefixed by type
//! of coordinate system used:
//! - Intrinsic: rotations are made around axes of rotating
//!   coordinate system associated with the object
//! - Extrinsic: rotations are made around axes of fixed
//!   (static) coordinate system
//!
//! Two specific values are provided for most frequently used
//! conventions: classic Euler angles (intrinsic ZXZ) and
//! yaw-pitch-roll (intrinsic ZYX).

enum gp_EulerSequence
{
  //! Classic Euler angles, alias to Intrinsic_ZXZ
  gp_EulerAngles,

  //! Yaw Pitch Roll (or nautical) angles, alias to Intrinsic_ZYX
  gp_YawPitchRoll,

  // Tait-Bryan angles (using three different axes)
  gp_Extrinsic_XYZ,
  gp_Extrinsic_XZY,
  gp_Extrinsic_YZX,
  gp_Extrinsic_YXZ,
  gp_Extrinsic_ZXY,
  gp_Extrinsic_ZYX,

  gp_Intrinsic_XYZ,
  gp_Intrinsic_XZY,
  gp_Intrinsic_YZX,
  gp_Intrinsic_YXZ,
  gp_Intrinsic_ZXY,
  gp_Intrinsic_ZYX,

  // Proper Euler angles (using two different axes, first and third the same)
  gp_Extrinsic_XYX,
  gp_Extrinsic_XZX,
  gp_Extrinsic_YZY,
  gp_Extrinsic_YXY,
  gp_Extrinsic_ZYZ,
  gp_Extrinsic_ZXZ,

  gp_Intrinsic_XYX,
  gp_Intrinsic_XZX,
  gp_Intrinsic_YZY,
  gp_Intrinsic_YXY,
  gp_Intrinsic_ZXZ,
  gp_Intrinsic_ZYZ
};

#endif // _gp_EulerSequence_HeaderFile
