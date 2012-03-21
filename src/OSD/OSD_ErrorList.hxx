// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef List_Of_Errors
#define List_Of_Errors
#include <OSD_WhoAmI.hxx>
#include <errno.h>

// List of OSD error codes

#define ERR_SURPRISE -2 
// Error message management didn't follow code evolution

#define ERR_UNKNOWN -1  
// Unknown system error

#define ERR_NONE     0  
// No error

// Errors common to alot of classes

#define ERR_ACCESS       1
#define ERR_EXIST        2
#define ERR_FAULT        3
#define ERR_INTR         4
#define ERR_INVAL        5
#define ERR_IO           6
#define ERR_ISDIR        7
#define ERR_NAMETOOLONG  8
#define ERR_NOENT        9
#define ERR_NOMEM       10
#define ERR_NOTDIR      11
#define ERR_PERM        12
#define ERR_QUOT        13
#define ERR_RANGE       14
#define ERR_ROFS        15
#define ERR_TOOBIG      16

//------------------- Error list by class Family ------------------------

// Class Directory

#define ERR_DMLINK      17
#define ERR_DNOENT      18

// Class File

#define ERR_FAGAIN      19
#define ERR_FBADF       20
#define ERR_FBADMSG     21
#define ERR_FDEADLK     22
#define ERR_FEXIST      23
#define ERR_FFBIG       24
#define ERR_FINVAL      25
#define ERR_FIO         26
#define ERR_FLOCKED     27
#define ERR_FMFILE      28
#define ERR_FNOLCK      29
#define ERR_FPERM       30
#define ERR_FRANGE      31
#define ERR_FWFD        32

// Class FileNode

#define ERR_FNBUSY      33
#define ERR_FNFILE      34
#define ERR_FNINVAL     35
#define ERR_FNOSPC      36
#define ERR_FNNOTEMPTY  37
#define ERR_FNXDEV      38

// Class MailBox

#define ERR_MBADF       40
#define ERR_MINVAL      41
#define ERR_MMFILE      42
#define ERR_MPERM       43
#define ERR_MSRCH       44

// Class Semaphore

#define ERR_SIDRM       45
#define ERR_SFBIG       46

// Class SharedMemory

#define ERR_SMMFILE     47
#define ERR_SMNOSPC     48
#define ERR_SNOENT      49

// Package
#define ERR_PPERM       50


#endif
