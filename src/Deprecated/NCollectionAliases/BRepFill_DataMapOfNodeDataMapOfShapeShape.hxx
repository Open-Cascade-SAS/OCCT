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

//! @file BRepFill_DataMapOfNodeDataMapOfShapeShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRepFill_DataMapOfNodeDataMapOfShapeShape_hxx
#define _BRepFill_DataMapOfNodeDataMapOfShapeShape_hxx

#include <Standard_Macro.hxx>
#include <MAT_Node.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("BRepFill_DataMapOfNodeDataMapOfShapeShape.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BRepFill_DataMapOfNodeDataMapOfShapeShape is deprecated, use NCollection_DataMap<opencascade::handle<MAT_Node>, TopTools_DataMapOfShapeShape> directly")
typedef NCollection_DataMap<opencascade::handle<MAT_Node>, TopTools_DataMapOfShapeShape> BRepFill_DataMapOfNodeDataMapOfShapeShape;
Standard_DEPRECATED("BRepFill_DataMapIteratorOfDataMapOfNodeDataMapOfShapeShape is deprecated, use NCollection_DataMap<opencascade::handle<MAT_Node>, TopTools_DataMapOfShapeShape>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<MAT_Node>, TopTools_DataMapOfShapeShape>::Iterator BRepFill_DataMapIteratorOfDataMapOfNodeDataMapOfShapeShape;

#endif // _BRepFill_DataMapOfNodeDataMapOfShapeShape_hxx
