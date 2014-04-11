// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _Standard_Time_HeaderFile
#define _Standard_Time_HeaderFile

#include <Standard_TypeDef.hxx>

// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - IsEqual
// ===============================================

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two time values are equal
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual (const Standard_Time theOne,
                                 const Standard_Time theTwo)
{
  return theOne == theTwo;
}

#endif
