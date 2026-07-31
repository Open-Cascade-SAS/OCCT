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

//! @file TopOpeBRepTool_DataMapOfOrientedShapeC2DF.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _TopOpeBRepTool_DataMapOfOrientedShapeC2DF_hxx
#define _TopOpeBRepTool_DataMapOfOrientedShapeC2DF_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_C2DF.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("TopOpeBRepTool_DataMapOfOrientedShapeC2DF.hxx is deprecated since OCCT "
                           "8.0.0. Use NCollection types directly.")

  Standard_DEPRECATED("TopOpeBRepTool_DataMapOfOrientedShapeC2DF is deprecated, use "
                      "NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_C2DF> directly")
typedef NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_C2DF>
  TopOpeBRepTool_DataMapOfOrientedShapeC2DF;
Standard_DEPRECATED("TopOpeBRepTool_DataMapIteratorOfDataMapOfOrientedShapeC2DF is deprecated, use "
                    "NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_C2DF>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, TopOpeBRepTool_C2DF>::Iterator
  TopOpeBRepTool_DataMapIteratorOfDataMapOfOrientedShapeC2DF;

#endif // _TopOpeBRepTool_DataMapOfOrientedShapeC2DF_hxx
