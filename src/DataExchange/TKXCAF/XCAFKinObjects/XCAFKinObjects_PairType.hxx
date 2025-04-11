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

//! Enumeration defining types of kinematic pairs that represent different kinds of joints
//! and constraints between rigid bodies in kinematic systems.
//! The enumeration includes three major categories:
//! - Low order pairs: Simple mechanical joints with well-defined degrees of freedom (DOF)
//! - Low order pairs with motion coupling: Joints where movement in one direction causes movement
//! in another
//! - High order pairs: Complex joints with advanced surface or curve interactions
enum XCAFKinObjects_PairType
{
  // Auxiliary type
  //! Default value representing no specific type
  XCAFKinObjects_PairType_NoType,

  // Low order pairs
  //! Rigid connection with no degrees of freedom (DOF) between connected bodies
  XCAFKinObjects_PairType_FullyConstrained,
  //! Connection with single rotational DOF around one axis (like a door hinge)
  XCAFKinObjects_PairType_Revolute,
  //! Connection with single translational DOF along one axis (like a sliding drawer)
  XCAFKinObjects_PairType_Prismatic,
  //! Connection with one rotational and one translational DOF along same axis (like a screw without
  //! pitch)
  XCAFKinObjects_PairType_Cylindrical,
  //! Connection with two rotational DOFs around different axes (like a universal joint)
  XCAFKinObjects_PairType_Universal,
  //! Similar to universal joint but with constant velocity in rotation (like a CV joint in
  //! vehicles)
  XCAFKinObjects_PairType_Homokinetic,
  //! Connection with two rotational DOFs with a pin limiting the third rotation
  XCAFKinObjects_PairType_SphericalWithPin,
  //! Connection with three rotational DOFs around a point (like a ball-and-socket joint)
  XCAFKinObjects_PairType_Spherical,
  //! Connection with one rotational and two translational DOFs (like an object sliding on a plane)
  XCAFKinObjects_PairType_Planar,
  //! Connection with no constraints - three rotational and three translational DOFs
  XCAFKinObjects_PairType_Unconstrained,

  // Low order pairs with motion coupling
  //! Connection where rotation causes translation along same axis with specified pitch (like a
  //! leadscrew)
  XCAFKinObjects_PairType_Screw,
  //! Connection where linear motion of rack causes rotation of pinion and vice versa
  XCAFKinObjects_PairType_RackAndPinion,
  //! Connection where rotation of one gear causes rotation of another with specified gear ratio
  XCAFKinObjects_PairType_Gear,
  //! Connection with flexible linear element coupled to pinion rotation (like a belt drive)
  XCAFKinObjects_PairType_LinearFlexibleAndPinion,

  // High order pairs
  //! Connection constraining a point to move on a surface
  XCAFKinObjects_PairType_PointOnSurface,
  //! Connection where two surfaces slide against each other
  XCAFKinObjects_PairType_SlidingSurface,
  //! Connection where two surfaces roll against each other without sliding
  XCAFKinObjects_PairType_RollingSurface,
  //! Connection constraining a point to move along a planar curve
  XCAFKinObjects_PairType_PointOnPlanarCurve,
  //! Connection where two curves slide against each other
  XCAFKinObjects_PairType_SlidingCurve,
  //! Connection where two curves roll against each other without sliding
  XCAFKinObjects_PairType_RollingCurve,
  //! Connection with flexible linear element constrained to move along a planar curve
  XCAFKinObjects_PairType_LinearFlexibleAndPlanarCurve
};

#endif // _XCAFKinObjects_PairType_HeaderFile
