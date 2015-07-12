// Created on: 1996-04-30
// Created by: cle
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Storage_OpenMode_HeaderFile
#define _Storage_OpenMode_HeaderFile


//! Specifies opening modes for a file:
//! -   Storage_VSNone : no mode is specified
//! -   Storage_VSRead : the file is open for  reading operations
//! -   Storage_VSWrite : the file is open for writing operations
//! -   Storage_VSReadWrite : the file is open
//! for both reading and writing operations.
enum Storage_OpenMode
{
Storage_VSNone,
Storage_VSRead,
Storage_VSWrite,
Storage_VSReadWrite
};

#endif // _Storage_OpenMode_HeaderFile
