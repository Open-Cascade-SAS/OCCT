// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _SelectMgr_SelectionType_HeaderFile
#define _SelectMgr_SelectionType_HeaderFile

//! Possible selection types
enum SelectMgr_SelectionType
{
  SelectMgr_SelectionType_Unknown = -1, //!< undefined selection type
  SelectMgr_SelectionType_Point,        //!< selection by point (frustum with some tolerance or axis)
  SelectMgr_SelectionType_Box,          //!< rectangle selection
  SelectMgr_SelectionType_Polyline      //!< polygonal selection
};

#endif // _SelectMgr_SelectionType_HeaderFile
