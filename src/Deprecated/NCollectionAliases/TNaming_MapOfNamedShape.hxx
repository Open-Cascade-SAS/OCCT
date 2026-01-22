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

//! @file TNaming_MapOfNamedShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TNaming_MapOfNamedShape_hxx
#define _TNaming_MapOfNamedShape_hxx

#include <Standard_Macro.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_Map.hxx>

Standard_HEADER_DEPRECATED("TNaming_MapOfNamedShape.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TNaming_MapOfNamedShape is deprecated, use NCollection_Map<opencascade::handle<TNaming_NamedShape>> directly")
typedef NCollection_Map<opencascade::handle<TNaming_NamedShape>> TNaming_MapOfNamedShape;
Standard_DEPRECATED("TNaming_MapIteratorOfMapOfNamedShape is deprecated, use NCollection_Map<opencascade::handle<TNaming_NamedShape>>::Iterator directly")
typedef NCollection_Map<opencascade::handle<TNaming_NamedShape>>::Iterator TNaming_MapIteratorOfMapOfNamedShape;

#endif // _TNaming_MapOfNamedShape_hxx
