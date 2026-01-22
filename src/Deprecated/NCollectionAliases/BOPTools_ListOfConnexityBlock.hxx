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

//! @file BOPTools_ListOfConnexityBlock.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BOPTools_ListOfConnexityBlock_hxx
#define _BOPTools_ListOfConnexityBlock_hxx

#include <Standard_Macro.hxx>
#include <NCollection_List.hxx>
#include <BOPTools_ConnexityBlock.hxx>

Standard_HEADER_DEPRECATED("BOPTools_ListOfConnexityBlock.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection types directly.")

  Standard_DEPRECATED("BOPTools_ListOfConnexityBlock is deprecated, use "
                      "NCollection_List<BOPTools_ConnexityBlock> directly")
typedef NCollection_List<BOPTools_ConnexityBlock> BOPTools_ListOfConnexityBlock;
Standard_DEPRECATED("BOPTools_ListIteratorOfListOfConnexityBlock is deprecated, use "
                    "NCollection_List<BOPTools_ConnexityBlock>::Iterator directly")
typedef NCollection_List<BOPTools_ConnexityBlock>::Iterator
  BOPTools_ListIteratorOfListOfConnexityBlock;

#endif // _BOPTools_ListOfConnexityBlock_hxx
