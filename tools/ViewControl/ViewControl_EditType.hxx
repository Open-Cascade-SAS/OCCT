// Created on: 2020-01-25
// Created by: Natalia ERMOLAEVA
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

#ifndef ViewControl_EditType_H
#define ViewControl_EditType_H

//! Type of context used in a tool library
enum ViewControl_EditType
{
  ViewControl_EditType_None,    //!< View widget is null
  ViewControl_EditType_Bool,    //!< check box widget
  ViewControl_EditType_Color,   //!< color selector widget
  ViewControl_EditType_Double,  //!< line edit widget used double validator
  ViewControl_EditType_Line,    //!< line edit widget
  ViewControl_EditType_Spin,    //!< spin box widget
  ViewControl_EditType_DoAction //!< control to perform the row action
};

#endif
