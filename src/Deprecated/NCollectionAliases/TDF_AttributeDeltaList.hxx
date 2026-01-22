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

//! @file TDF_AttributeDeltaList.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TDF_AttributeDeltaList_hxx
#define _TDF_AttributeDeltaList_hxx

#include <Standard_Macro.hxx>
#include <TDF_AttributeDelta.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED("TDF_AttributeDeltaList.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TDF_AttributeDeltaList is deprecated, use NCollection_List<opencascade::handle<TDF_AttributeDelta>> directly")
typedef NCollection_List<opencascade::handle<TDF_AttributeDelta>> TDF_AttributeDeltaList;
Standard_DEPRECATED("TDF_ListIteratorOfAttributeDeltaList is deprecated, use NCollection_List<opencascade::handle<TDF_AttributeDelta>>::Iterator directly")
typedef NCollection_List<opencascade::handle<TDF_AttributeDelta>>::Iterator TDF_ListIteratorOfAttributeDeltaList;

#endif // _TDF_AttributeDeltaList_hxx
