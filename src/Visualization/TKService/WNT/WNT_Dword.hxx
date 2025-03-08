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

#ifndef WNT_Dword_HeaderFile
#define WNT_Dword_HeaderFile

// Purpose: Defines a Windows NT DWORD type.

#ifndef __WINDOWS_H_INCLUDED
  #define __WINDOWS_H_INCLUDED
  #ifndef STRICT
    #define STRICT
  #endif /* STRICT */
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

  #ifdef DrawText
    #undef DrawText
  #endif

  #ifdef THIS
    #undef THIS
  #endif // THIS
#endif   // __WINDOWS_H_INCLUDED

typedef DWORD WNT_Dword;

#endif // __WNT_Dword_HeaderFile
