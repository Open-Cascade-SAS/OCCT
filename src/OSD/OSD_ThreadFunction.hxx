// Created on: 2006-03-10
// Created by: data exchange team
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

#ifndef OSD_ThreadFunction_HeaderFile
#define OSD_ThreadFunction_HeaderFile

#include <Standard_Address.hxx>

//! Typedef for prototype of function to be used as main function of a thread.
//!
//! Note: currently we use the same prototype for thread functions on all platforms,
//! in order to make user programs less platform-dependent.
//! However, there is a distinction in returned value for the thread function
//! on UNIX/Linux (void*) and Windows (DWORD) systems.
//! Thus on Windows we have to encode returned void* as DWORD.
//! It is OK for WIN32, but potentially problem on WIN64.
//! To avoid any problems with this, for better application portability it is recommended
//! that the thread function returns just integer (casted to void*).
//! This shall work on all platforms.
typedef Standard_Address (*OSD_ThreadFunction) (Standard_Address data);

//#ifdef _WIN32
//typedef LPTHREAD_START_ROUTINE OSD_ThreadFunction;
//#endif

#endif
