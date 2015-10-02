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

#ifndef Standard_ThreadId_HeaderFile
#define Standard_ThreadId_HeaderFile

// Platform-dependent definition of the thread identifier type

#ifdef _WIN32

#include <windows.h>
typedef DWORD Standard_ThreadId;

inline Standard_Integer HashCode(const Standard_ThreadId Value,
  const Standard_Integer Upper)
{
  // Size of int == size of unsigned long == 4 for WIN32 and WIN64
  return HashCode((Standard_Integer)Value, Upper);
}

#else

#include <pthread.h>
typedef pthread_t Standard_ThreadId;

#endif

#endif
