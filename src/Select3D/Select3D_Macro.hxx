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

#ifndef _Select3D_Macro_HeaderFile
#define _Select3D_Macro_HeaderFile

#include <Standard_ShortReal.hxx>

// Safe conversion of Standard_ShortReal(float) to Standard_Real(double)
inline Standard_ShortReal DToF (Standard_Real a) 
{
  return a > ShortRealLast()  ? ShortRealLast()  :
         a < ShortRealFirst() ? ShortRealFirst() : (Standard_ShortReal)a;
}

#endif
