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

//! @file TopOpeBRepBuild_ListOfListOfLoop.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TopOpeBRepBuild_ListOfListOfLoop_hxx
#define _TopOpeBRepBuild_ListOfListOfLoop_hxx

#include <Standard_Macro.hxx>
#include <TopOpeBRepBuild_ListOfLoop.hxx>
#include <NCollection_List.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepBuild_ListOfListOfLoop.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TopOpeBRepBuild_ListOfListOfLoop is deprecated, use NCollection_List<TopOpeBRepBuild_ListOfLoop> directly")
typedef NCollection_List<TopOpeBRepBuild_ListOfLoop> TopOpeBRepBuild_ListOfListOfLoop;
Standard_DEPRECATED("TopOpeBRepBuild_ListIteratorOfListOfListOfLoop is deprecated, use NCollection_List<TopOpeBRepBuild_ListOfLoop>::Iterator directly")
typedef NCollection_List<TopOpeBRepBuild_ListOfLoop>::Iterator TopOpeBRepBuild_ListIteratorOfListOfListOfLoop;

#endif // _TopOpeBRepBuild_ListOfListOfLoop_hxx
