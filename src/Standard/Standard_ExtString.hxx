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

//============================================================================
//==== Title: Standard_ExtString.hxx
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef short* Standard_ExtString;
//============================================================================

#ifndef _Standard_ExtString_HeaderFile
#define _Standard_ExtString_HeaderFile

#include <Standard_TypeDef.hxx>

//! Computes a hash code for the given wide character string, in the range [1, theUpperBound]
//! @param theExtString the wide character string which hash code is to be computed
//! @param theUpperBound the upper bound of the range a computing hash code must be within
//! @return a computed hash code, in the range [1, theUpperBound]
Standard_EXPORT Standard_Integer HashCode (Standard_ExtString theExtString, Standard_Integer theUpperBound);

#endif
