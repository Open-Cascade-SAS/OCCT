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

#ifndef _Standard_DefineHandle_HeaderFile
#define _Standard_DefineHandle_HeaderFile

//! @file
//! This file provides obsolete low-level helper macros used to define OCCT handles and types,
//! for compatibility with previous versions of OCCT.
//! Since OCCT 7.0, relevant macros are provided by Standard_Type.hxx and Standard_Handle.hxx.

#include <Standard_Type.hxx>

class Standard_Transient;
class Standard_Persistent;
class Standard_Type;

// Obsolete macros kept for compatibility
#define IMPLEMENT_DOWNCAST(C1, BC)
#define IMPLEMENT_STANDARD_HANDLE(C1, C2)
#define IMPLEMENT_STANDARD_PHANDLE(C1, C2)
#define IMPLEMENT_STANDARD_RTTI(C1)
#define IMPLEMENT_STANDARD_TYPE(C1)
#define IMPLEMENT_STANDARD_SUPERTYPE(Cn)
#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
#define IMPLEMENT_STANDARD_TYPE_END(C1)

#endif
