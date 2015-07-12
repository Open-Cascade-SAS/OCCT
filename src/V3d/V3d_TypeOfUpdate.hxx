// Created on: 1992-11-13
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _V3d_TypeOfUpdate_HeaderFile
#define _V3d_TypeOfUpdate_HeaderFile

//! Determines the type of update of the view
//! -   V3d_ASAP: as soon as possible. The view is updated immediately after a modification.
//! -   V3d_WAIT: deferred. The view is updated when the Update function is called.
enum V3d_TypeOfUpdate
{
V3d_ASAP,
V3d_WAIT
};

#endif // _V3d_TypeOfUpdate_HeaderFile
