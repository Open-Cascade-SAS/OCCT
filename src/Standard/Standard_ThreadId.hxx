// File:	Standard_ThreadId.hxx
// Created:	Fri Mar 10 17:00:00 2006
// Author:	data exchange team
//		<det@decex.nnov.opencascade.com>


#ifndef Standard_ThreadId_HeaderFile
#define Standard_ThreadId_HeaderFile

// Platform-dependent definition of the thread identifier type

#ifdef WNT

#include <windows.h>
typedef DWORD Standard_ThreadId;

#else

#include <pthread.h>
typedef pthread_t Standard_ThreadId;

#endif

#endif
