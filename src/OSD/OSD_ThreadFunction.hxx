// File:	OSD_ThreadFunction.hxx
// Created:	Fri Mar 10 17:00:00 2006
// Author:	data exchange team
//		<det@decex.nnov.opencascade.com>


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
