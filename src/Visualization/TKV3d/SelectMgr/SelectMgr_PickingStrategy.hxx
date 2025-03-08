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

#ifndef _SelectMgr_PickingStrategy_HeaderFile
#define _SelectMgr_PickingStrategy_HeaderFile

//! Enumeration defines picking strategy - which entities detected by picking line will be accepted,
//! considering selection filters.
enum SelectMgr_PickingStrategy
{
  SelectMgr_PickingStrategy_FirstAcceptable, //!< the first detected entity passing selection filter
                                             //!< is accepted (e.g. any)
  SelectMgr_PickingStrategy_OnlyTopmost //!< only topmost detected entity passing selection filter
                                        //!< is accepted
};

#endif // _SelectMgr_PickingStrategy_HeaderFile
