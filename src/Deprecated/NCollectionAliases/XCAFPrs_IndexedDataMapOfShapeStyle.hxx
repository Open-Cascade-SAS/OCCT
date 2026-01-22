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

//! @file XCAFPrs_IndexedDataMapOfShapeStyle.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _XCAFPrs_IndexedDataMapOfShapeStyle_hxx
#define _XCAFPrs_IndexedDataMapOfShapeStyle_hxx

#include <Standard_Macro.hxx>
#include <XCAFPrs_Style.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>

Standard_HEADER_DEPRECATED("XCAFPrs_IndexedDataMapOfShapeStyle.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("XCAFPrs_IndexedDataMapOfShapeStyle is deprecated, use NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher> directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher> XCAFPrs_IndexedDataMapOfShapeStyle;
Standard_DEPRECATED("XCAFPrs_DataMapIteratorOfIndexedDataMapOfShapeStyle is deprecated, use NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_IndexedDataMap<TopoDS_Shape, XCAFPrs_Style, TopTools_ShapeMapHasher>::Iterator XCAFPrs_DataMapIteratorOfIndexedDataMapOfShapeStyle;

#endif // _XCAFPrs_IndexedDataMapOfShapeStyle_hxx
