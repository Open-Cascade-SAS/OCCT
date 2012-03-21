// Created on: 2006-03-10
// Created by: data exchange team
// Copyright (c) 2006-2012 OPEN CASCADE SAS
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



#ifndef OSD_ThreadFunction_HeaderFile
#define OSD_ThreadFunction_HeaderFile

#include <Standard_Address.hxx>

// Typedef for prototype of function to be used as main 
// function of a thread 

// Note: currently we use the same prototype for thread functions on all 
// platforms, in order to make user programs less platform-dependent.
// However, there is a distinction in returned value for the thread function
// on UNIX/Linux (void*) and WIndows (DWORD) systems. 
// Thus on Windows we have to encode returned void* as DWORD. It is OK for WIN32,
// but potentially problem on WIN64. 
// To avoid any problems with this, for better application portability it is recomended 
// that the thread function returns just integer (casted to void*). 
// This shall work on all platforms.

//#ifdef WNT
//#include <windows.h>
//typedef LPTHREAD_START_ROUTINE OSD_ThreadFunction;
//#else

typedef Standard_Address (*OSD_ThreadFunction) (Standard_Address data);

//#endif

#endif
