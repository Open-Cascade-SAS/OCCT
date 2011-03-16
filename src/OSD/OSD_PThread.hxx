// File:	OSD_PThread.hxx
// Created:	Fri Mar 10 17:00:00 2006
// Author:	data exchange team
//		<det@decex.nnov.opencascade.com>


#ifndef OSD_PThread_HeaderFile
#define OSD_PThread_HeaderFile

// Platform-dependent definition of the thread handle type

#ifdef WNT

#include <windows.h>
typedef HANDLE OSD_PThread;

#else

#include <pthread.h>
typedef pthread_t OSD_PThread;

#endif

#endif
