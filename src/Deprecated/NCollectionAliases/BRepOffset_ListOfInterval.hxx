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

//! @file BRepOffset_ListOfInterval.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRepOffset_ListOfInterval_hxx
#define _BRepOffset_ListOfInterval_hxx

#include <Standard_Macro.hxx>
#include <BRepOffset_Interval.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED("BRepOffset_ListOfInterval.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BRepOffset_ListOfInterval is deprecated, use NCollection_List<BRepOffset_Interval> directly")
typedef NCollection_List<BRepOffset_Interval> BRepOffset_ListOfInterval;
Standard_DEPRECATED("BRepOffset_ListIteratorOfListOfInterval is deprecated, use NCollection_List<BRepOffset_Interval>::Iterator directly")
typedef NCollection_List<BRepOffset_Interval>::Iterator BRepOffset_ListIteratorOfListOfInterval;

#endif // _BRepOffset_ListOfInterval_hxx
