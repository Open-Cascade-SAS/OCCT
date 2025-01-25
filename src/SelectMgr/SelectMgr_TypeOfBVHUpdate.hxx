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

#ifndef _SelectMgr_TypeOfBVHUpdate_HeaderFile
#define _SelectMgr_TypeOfBVHUpdate_HeaderFile

//! Keeps track for BVH update state for each SelectMgr_Selection entity in a following way:
//! - Add        : 2nd level BVH does not contain any of the selection's sensitive entities and they
//! must be added;
//! - Remove     : all sensitive entities of the selection must be removed from 2nd level BVH;
//! - Renew      : 2nd level BVH already contains sensitives of the selection, but the its complete
//! update and removal is required. Therefore, sensitives of the selection with this type of update
//! must be removed from 2nd level BVH and added after recomputation.
//! - Invalidate : the 2nd level BVH needs to be rebuilt;
//! - None       : entities of the selection are up to date.
enum SelectMgr_TypeOfBVHUpdate
{
  SelectMgr_TBU_Add,
  SelectMgr_TBU_Remove,
  SelectMgr_TBU_Renew,
  SelectMgr_TBU_Invalidate,
  SelectMgr_TBU_None
};

#endif // _SelectMgr_TypeOfBVHUpdate_HeaderFile
