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

//! @file TopOpeBRepTool_IndexedDataMapOfShapeBox.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box> directly instead.

#ifndef _TopOpeBRepTool_IndexedDataMapOfShapeBox_hxx
#define _TopOpeBRepTool_IndexedDataMapOfShapeBox_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepTool_IndexedDataMapOfShapeBox.hxx is deprecated since OCCT "
                           "8.0.0. Use NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box> directly.")

  Standard_DEPRECATED("TopOpeBRepTool_IndexedDataMapOfShapeBox is deprecated, use "
                      "NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, Bnd_Box> TopOpeBRepTool_IndexedDataMapOfShapeBox;

#endif // _TopOpeBRepTool_IndexedDataMapOfShapeBox_hxx
