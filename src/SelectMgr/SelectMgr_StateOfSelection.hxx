// Created on: 1995-02-06
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _SelectMgr_StateOfSelection_HeaderFile
#define _SelectMgr_StateOfSelection_HeaderFile

//! different state of a Selection in a ViewerSelector...
enum SelectMgr_StateOfSelection
{
  SelectMgr_SOS_Any     = -2, //!< ANY selection state (for querying selections)
  SelectMgr_SOS_Unknown = -1, //!< selection, which has  never been in SelectMgr_SOS_Activated state
                              //!< (almost the same thing as SelectMgr_SOS_Deactivated)
  SelectMgr_SOS_Deactivated =
    0,                     //!< deactivated selection, once been in SelectMgr_SOS_Activated state
  SelectMgr_SOS_Activated, //!< activated selection
};

#endif // _SelectMgr_StateOfSelection_HeaderFile
