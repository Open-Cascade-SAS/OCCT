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

//! @file BRepCheck_ListOfStatus.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRepCheck_ListOfStatus_hxx
#define _BRepCheck_ListOfStatus_hxx

#include <Standard_Macro.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>

Standard_HEADER_DEPRECATED("BRepCheck_ListOfStatus.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BRepCheck_ListOfStatus is deprecated, use NCollection_List<BRepCheck_Status> directly")
typedef NCollection_List<BRepCheck_Status> BRepCheck_ListOfStatus;
Standard_DEPRECATED("BRepCheck_HListOfStatus is deprecated, use NCollection_Shared<BRepCheck_ListOfStatus> directly")
typedef NCollection_Shared<BRepCheck_ListOfStatus> BRepCheck_HListOfStatus;
Standard_DEPRECATED("BRepCheck_ListIteratorOfListOfStatus is deprecated, use NCollection_List<BRepCheck_Status>::Iterator directly")
typedef NCollection_List<BRepCheck_Status>::Iterator BRepCheck_ListIteratorOfListOfStatus;

#endif // _BRepCheck_ListOfStatus_hxx
