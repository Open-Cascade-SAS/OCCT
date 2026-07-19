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

//! @file DBRep_ListOfEdge.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _DBRep_ListOfEdge_hxx
#define _DBRep_ListOfEdge_hxx

#include <Standard_Macro.hxx>
#include <DBRep_Edge.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED(
  "DBRep_ListOfEdge.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("DBRep_ListOfEdge is deprecated, use "
                      "NCollection_List<opencascade::handle<DBRep_Edge>> directly")
typedef NCollection_List<opencascade::handle<DBRep_Edge>> DBRep_ListOfEdge;
Standard_DEPRECATED("DBRep_ListIteratorOfListOfEdge is deprecated, use "
                    "NCollection_List<opencascade::handle<DBRep_Edge>>::Iterator directly")
typedef NCollection_List<opencascade::handle<DBRep_Edge>>::Iterator DBRep_ListIteratorOfListOfEdge;

#endif // _DBRep_ListOfEdge_hxx
