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

//! @file CDM_MapOfDocument.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _CDM_MapOfDocument_hxx
#define _CDM_MapOfDocument_hxx

#include <Standard_Macro.hxx>
#include <CDM_Document.hxx>
#include <NCollection_Map.hxx>

Standard_HEADER_DEPRECATED("CDM_MapOfDocument.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("CDM_MapOfDocument is deprecated, use NCollection_Map<opencascade::handle<CDM_Document>> directly")
typedef NCollection_Map<opencascade::handle<CDM_Document>> CDM_MapOfDocument;
Standard_DEPRECATED("CDM_MapIteratorOfMapOfDocument is deprecated, use NCollection_Map<opencascade::handle<CDM_Document>>::Iterator directly")
typedef NCollection_Map<opencascade::handle<CDM_Document>>::Iterator CDM_MapIteratorOfMapOfDocument;

#endif // _CDM_MapOfDocument_hxx
