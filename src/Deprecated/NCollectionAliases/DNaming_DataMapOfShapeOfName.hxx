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

//! @file DNaming_DataMapOfShapeOfName.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _DNaming_DataMapOfShapeOfName_hxx
#define _DNaming_DataMapOfShapeOfName_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("DNaming_DataMapOfShapeOfName.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection types directly.")

  Standard_DEPRECATED(
    "DNaming_DataMapOfShapeOfName is deprecated, use NCollection_DataMap<TopoDS_Shape, "
    "TCollection_AsciiString, TopTools_ShapeMapHasher> directly")
typedef NCollection_DataMap<TopoDS_Shape, TCollection_AsciiString, TopTools_ShapeMapHasher>
  DNaming_DataMapOfShapeOfName;
Standard_DEPRECATED("DNaming_DataMapIteratorOfDataMapOfShapeOfName is deprecated, use "
                    "NCollection_DataMap<TopoDS_Shape, TCollection_AsciiString, "
                    "TopTools_ShapeMapHasher>::Iterator directly")
typedef NCollection_DataMap<TopoDS_Shape, TCollection_AsciiString, TopTools_ShapeMapHasher>::
  Iterator DNaming_DataMapIteratorOfDataMapOfShapeOfName;

#endif // _DNaming_DataMapOfShapeOfName_hxx
