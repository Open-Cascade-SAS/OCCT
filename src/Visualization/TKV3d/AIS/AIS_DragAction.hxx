// Copyright (c) 2018-2019 OPEN CASCADE SAS
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

#ifndef _AIS_DragAction_HeaderFile
#define _AIS_DragAction_HeaderFile

//! Dragging action.
enum AIS_DragAction
{
  AIS_DragAction_Start,     //!< (try) start dragging object
  AIS_DragAction_Confirmed, //!< dragging interaction is confirmed.
  AIS_DragAction_Update,    //!< perform dragging (update position)
  AIS_DragAction_Stop,      //!< stop dragging (save position)
  AIS_DragAction_Abort,     //!< abort dragging (restore initial position)
};

#endif // _AIS_DragAction_HeaderFile
