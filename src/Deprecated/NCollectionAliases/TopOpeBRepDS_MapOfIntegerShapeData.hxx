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

//! @file TopOpeBRepDS_MapOfIntegerShapeData.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TopOpeBRepDS_MapOfIntegerShapeData_hxx
#define _TopOpeBRepDS_MapOfIntegerShapeData_hxx

#include <Standard_Macro.hxx>
#include <TopOpeBRepDS_ShapeData.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepDS_MapOfIntegerShapeData.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("TopOpeBRepDS_MapOfIntegerShapeData is deprecated, use NCollection_DataMap<int, TopOpeBRepDS_ShapeData> directly")
typedef NCollection_DataMap<int, TopOpeBRepDS_ShapeData> TopOpeBRepDS_MapOfIntegerShapeData;
Standard_DEPRECATED("TopOpeBRepDS_DataMapIteratorOfMapOfIntegerShapeData is deprecated, use NCollection_DataMap<int, TopOpeBRepDS_ShapeData>::Iterator directly")
typedef NCollection_DataMap<int, TopOpeBRepDS_ShapeData>::Iterator TopOpeBRepDS_DataMapIteratorOfMapOfIntegerShapeData;

#endif // _TopOpeBRepDS_MapOfIntegerShapeData_hxx
