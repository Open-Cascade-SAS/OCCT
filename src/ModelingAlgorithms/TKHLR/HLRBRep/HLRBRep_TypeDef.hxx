// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef _HLRBRep_TypeDef_HeaderFile
#define _HLRBRep_TypeDef_HeaderFile

//! Type alias for curve pointer used in HLRBRep algorithms.
//! This provides a named type (void*) for curve pointers used in generic
//! algorithm interfaces, replacing the deprecated void*.
//! The void* type is required for compatibility with the generic template
//! instantiation system that uses const reference parameters.
typedef void* HLRBRep_CurvePtr;

//! Type alias for surface pointer used in HLRBRep algorithms.
//! This provides a named type (void*) for surface pointers used in generic
//! algorithm interfaces, replacing the deprecated void*.
typedef void* HLRBRep_SurfacePtr;

#endif // _HLRBRep_TypeDef_HeaderFile
