// Created on: 2011-05-17
// Created by: Oleg AGASHIN
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef _IMeshData_Status_HeaderFile
#define _IMeshData_Status_HeaderFile

//! Enumerates statuses used to notify state of discrete model.
enum IMeshData_Status
{
  IMeshData_NoError               = 0x0,   //!< Mesh generation is successful.
  IMeshData_OpenWire              = 0x1,   //!< Notifies open wire problem, which can potentially lead to incorrect results.
  IMeshData_SelfIntersectingWire  = 0x2,   //!< Notifies self-intersections on discretized wire, which can potentially lead to incorrect results.
  IMeshData_Failure               = 0x4,   //!< Failed to generate mesh for some faces.
  IMeshData_ReMesh                = 0x8,   //!< Deflection of some edges has been decreased due to interference of discrete model.
  IMeshData_UnorientedWire        = 0x10,  //!< Notifies bad orientation of a wire, which can potentially lead to incorrect results.
  IMeshData_TooFewPoints          = 0x20,  //!< Discrete model contains too few boundary points to generate mesh.
  IMeshData_Outdated              = 0x40,  //!< Existing triangulation of some faces corresponds to greater deflection than specified by parameter.
  IMeshData_Reused                = 0x80,  //!< Existing triangulation of some faces is reused as far as it fits specified deflection.
  IMeshData_UserBreak             = 0x100  //!< User break
};

#endif
