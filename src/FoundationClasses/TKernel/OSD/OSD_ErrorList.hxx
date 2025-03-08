// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef OSD_ErrorList_HeaderFile
#define OSD_ErrorList_HeaderFile
#include <OSD_WhoAmI.hxx>
#include <errno.h>

// List of OSD error codes

#define ERR_SURPRISE -2
// Error message management didn't follow code evolution

#define ERR_UNKNOWN -1
// Unknown system error

#define ERR_NONE 0
// No error

// Errors common to alot of classes

#define ERR_ACCESS 1
#define ERR_EXIST 2
#define ERR_FAULT 3
#define ERR_INTR 4
#define ERR_INVAL 5
#define ERR_IO 6
#define ERR_ISDIR 7
#define ERR_NAMETOOLONG 8
#define ERR_NOENT 9
#define ERR_NOMEM 10
#define ERR_NOTDIR 11
#define ERR_PERM 12
#define ERR_QUOT 13
#define ERR_RANGE 14
#define ERR_ROFS 15
#define ERR_TOOBIG 16

//------------------- Error list by class Family ------------------------

// Class Directory

#define ERR_DMLINK 17
#define ERR_DNOENT 18

// Class File

#define ERR_FAGAIN 19
#define ERR_FBADF 20
#define ERR_FBADMSG 21
#define ERR_FDEADLK 22
#define ERR_FEXIST 23
#define ERR_FFBIG 24
#define ERR_FINVAL 25
#define ERR_FIO 26
#define ERR_FLOCKED 27
#define ERR_FMFILE 28
#define ERR_FNOLCK 29
#define ERR_FPERM 30
#define ERR_FRANGE 31
#define ERR_FWFD 32

// Class FileNode

#define ERR_FNBUSY 33
#define ERR_FNFILE 34
#define ERR_FNINVAL 35
#define ERR_FNOSPC 36
#define ERR_FNNOTEMPTY 37
#define ERR_FNXDEV 38

// Package
#define ERR_PPERM 50

#endif
