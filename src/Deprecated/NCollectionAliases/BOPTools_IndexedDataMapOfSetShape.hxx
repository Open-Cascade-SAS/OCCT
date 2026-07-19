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

//! @file BOPTools_IndexedDataMapOfSetShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<BOPTools_Set, TopoDS_Shape> directly instead.

#ifndef _BOPTools_IndexedDataMapOfSetShape_hxx
#define _BOPTools_IndexedDataMapOfSetShape_hxx

#include <Standard_Macro.hxx>
#include <BOPTools_Set.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Shape.hxx>

Standard_HEADER_DEPRECATED("BOPTools_IndexedDataMapOfSetShape.hxx is deprecated since OCCT 8.0.0. "
                           "Use NCollection_IndexedDataMap<BOPTools_Set, TopoDS_Shape> directly.")

  Standard_DEPRECATED("BOPTools_IndexedDataMapOfSetShape is deprecated, use "
                      "NCollection_IndexedDataMap<BOPTools_Set, TopoDS_Shape> directly")
typedef NCollection_IndexedDataMap<BOPTools_Set, TopoDS_Shape> BOPTools_IndexedDataMapOfSetShape;

#endif // _BOPTools_IndexedDataMapOfSetShape_hxx
