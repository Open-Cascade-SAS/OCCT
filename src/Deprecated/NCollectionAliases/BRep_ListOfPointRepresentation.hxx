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

//! @file BRep_ListOfPointRepresentation.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRep_ListOfPointRepresentation_hxx
#define _BRep_ListOfPointRepresentation_hxx

#include <Standard_Macro.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED("BRep_ListOfPointRepresentation.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BRep_ListOfPointRepresentation is deprecated, use NCollection_List<opencascade::handle<BRep_PointRepresentation>> directly")
typedef NCollection_List<opencascade::handle<BRep_PointRepresentation>> BRep_ListOfPointRepresentation;
Standard_DEPRECATED("BRep_ListIteratorOfListOfPointRepresentation is deprecated, use NCollection_List<opencascade::handle<BRep_PointRepresentation>>::Iterator directly")
typedef NCollection_List<opencascade::handle<BRep_PointRepresentation>>::Iterator BRep_ListIteratorOfListOfPointRepresentation;

#endif // _BRep_ListOfPointRepresentation_hxx
