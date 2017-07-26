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

#ifndef View_ViewActionType_H
#define View_ViewActionType_H

//! Actions of view widget
enum View_ViewActionType
{
  View_ViewActionType_FitAllId,  //!< fits the view to wrap the bounding box of displayed objects
  View_ViewActionType_FitAreaId, //!< zoom the view to the given area
  View_ViewActionType_ZoomId,    //!< zoom in/out the view by some delta
  View_ViewActionType_PanId,     //!< move position of the view camera
  View_ViewActionType_RotationId, //!< rotate the view
  View_ViewActionType_DisplayModeId //!< change display mode of visualized presentations (to wireframe or shading)
};

#endif
