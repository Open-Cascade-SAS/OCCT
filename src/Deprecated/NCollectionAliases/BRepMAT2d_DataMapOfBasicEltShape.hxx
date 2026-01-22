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

//! @file BRepMAT2d_DataMapOfBasicEltShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _BRepMAT2d_DataMapOfBasicEltShape_hxx
#define _BRepMAT2d_DataMapOfBasicEltShape_hxx

#include <Standard_Macro.hxx>
#include <MAT_BasicElt.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("BRepMAT2d_DataMapOfBasicEltShape.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("BRepMAT2d_DataMapOfBasicEltShape is deprecated, use NCollection_DataMap<opencascade::handle<MAT_BasicElt>, TopoDS_Shape> directly")
typedef NCollection_DataMap<opencascade::handle<MAT_BasicElt>, TopoDS_Shape> BRepMAT2d_DataMapOfBasicEltShape;
Standard_DEPRECATED("BRepMAT2d_DataMapIteratorOfDataMapOfBasicEltShape is deprecated, use NCollection_DataMap<opencascade::handle<MAT_BasicElt>, TopoDS_Shape>::Iterator directly")
typedef NCollection_DataMap<opencascade::handle<MAT_BasicElt>, TopoDS_Shape>::Iterator BRepMAT2d_DataMapIteratorOfDataMapOfBasicEltShape;

#endif // _BRepMAT2d_DataMapOfBasicEltShape_hxx
