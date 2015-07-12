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

#ifndef _OSD_LockType_HeaderFile
#define _OSD_LockType_HeaderFile

//! locks for files.
//! NoLock is the default value when opening a file.
//!
//! ReadLock allows only one reading of the file at a time.
//!
//! WriteLock prevents others writing into a file(excepted the user
//! who puts the lock)but allows everybody to read.
//!
//! ExclusiveLock prevents reading and writing except for the
//! current user of the file.
//! So ExclusiveLock means only one user on the file and this
//! user is the one who puts the lock.
enum OSD_LockType
{
OSD_NoLock,
OSD_ReadLock,
OSD_WriteLock,
OSD_ExclusiveLock
};

#endif // _OSD_LockType_HeaderFile
