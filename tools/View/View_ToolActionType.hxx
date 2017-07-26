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

#ifndef View_ToolActionType_H
#define View_ToolActionType_H

//! Actions of view tool bar
enum View_ToolActionType
{
  View_ToolActionType_KeepViewId,    //!< Do not clear previously shown presentations
  View_ToolActionType_KeepViewOffId, //!< Do show only for new displayed presentation
  View_ToolActionType_ClearViewId    //!< Erase all displayed presentations
};

#endif
