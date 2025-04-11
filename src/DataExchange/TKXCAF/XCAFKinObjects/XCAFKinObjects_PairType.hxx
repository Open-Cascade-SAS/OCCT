// Created on: 2020-03-16
// Created by: Irina KRYLOVA
// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _XCAFKinObjects_PairType_HeaderFile
#define _XCAFKinObjects_PairType_HeaderFile

//! Defines types of kinemtic pairs
enum XCAFKinObjects_PairType
{
  // Auxiliary type
  XCAFKinObjects_PairType_NoType,

  // Low order pairs
  XCAFKinObjects_PairType_FullyConstrained, // no DOF
  XCAFKinObjects_PairType_Revolute,         // one rotation DOF
  XCAFKinObjects_PairType_Prismatic,        // one translation DOF
  XCAFKinObjects_PairType_Cylindrical,      // one rotation and one translation DOF
  XCAFKinObjects_PairType_Universal,        // two rotation DOF
  XCAFKinObjects_PairType_Homokinetic,      // two uniform rotation DOF
  XCAFKinObjects_PairType_SphericalWithPin, // two rotation DOF
  XCAFKinObjects_PairType_Spherical,        // three rotation DOF
  XCAFKinObjects_PairType_Planar,           // one rotation and two translation DOF
  XCAFKinObjects_PairType_Unconstrained,    // three rotation and three translation DOF

  // Low order pairs with motion coupling
  XCAFKinObjects_PairType_Screw,
  XCAFKinObjects_PairType_RackAndPinion,
  XCAFKinObjects_PairType_Gear,
  XCAFKinObjects_PairType_LinearFlexibleAndPinion,

  // High order pairs
  XCAFKinObjects_PairType_PointOnSurface,
  XCAFKinObjects_PairType_SlidingSurface,
  XCAFKinObjects_PairType_RollingSurface,
  XCAFKinObjects_PairType_PointOnPlanarCurve,
  XCAFKinObjects_PairType_SlidingCurve,
  XCAFKinObjects_PairType_RollingCurve,
  XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve
};

#endif // _XCAFKinObjects_PairType_HeaderFile
