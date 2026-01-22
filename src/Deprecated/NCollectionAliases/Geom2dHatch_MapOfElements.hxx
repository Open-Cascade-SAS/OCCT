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

//! @file Geom2dHatch_MapOfElements.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _Geom2dHatch_MapOfElements_hxx
#define _Geom2dHatch_MapOfElements_hxx

#include <Standard_Macro.hxx>
#include <Geom2dHatch_Element.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("Geom2dHatch_MapOfElements.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("Geom2dHatch_MapOfElements is deprecated, use NCollection_DataMap<int, Geom2dHatch_Element> directly")
typedef NCollection_DataMap<int, Geom2dHatch_Element> Geom2dHatch_MapOfElements;
Standard_DEPRECATED("Geom2dHatch_DataMapIteratorOfMapOfElements is deprecated, use NCollection_DataMap<int, Geom2dHatch_Element>::Iterator directly")
typedef NCollection_DataMap<int, Geom2dHatch_Element>::Iterator Geom2dHatch_DataMapIteratorOfMapOfElements;

#endif // _Geom2dHatch_MapOfElements_hxx
