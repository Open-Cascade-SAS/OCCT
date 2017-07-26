// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef VInspector_CallBackMode_H
#define VInspector_CallBackMode_H

//! History context modes corresponding to actions in AIS_InteractiveContext
enum VInspector_CallBackMode
{
  VInspector_CallBackMode_None, //!< undefined 
  VInspector_CallBackMode_Display, //!< Display
  VInspector_CallBackMode_Redisplay, //!< Redisplay
  VInspector_CallBackMode_Remove, //!< Remove
  VInspector_CallBackMode_Load, //!< Load
  VInspector_CallBackMode_ActivatedModes, //!< ActivatedModes
  VInspector_CallBackMode_Activate, //!< Activate
  VInspector_CallBackMode_Deactivate, //!< Deactivate
  VInspector_CallBackMode_AddOrRemoveSelectedShape, //!< AddOrRemoveSelectedShape for TopoDS_Shape
  VInspector_CallBackMode_AddOrRemoveSelected, //!< AddOrRemoveSelectedShape for Presentation
  VInspector_CallBackMode_AddOrRemoveSelectedOwner, //!< AddOrRemoveSelectedShape for Owner
  VInspector_CallBackMode_ClearSelected, //!< ClearSelected
  VInspector_CallBackMode_MoveTo, //!< MoveTo
  VInspector_CallBackMode_SetSelected, //!< SetSelected
  VInspector_CallBackMode_Select, //!< Select
  VInspector_CallBackMode_ShiftSelect //!< ShiftSelect
};

#endif 
