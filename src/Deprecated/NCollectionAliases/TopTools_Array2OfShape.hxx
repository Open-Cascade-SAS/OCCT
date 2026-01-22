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

//! @file TopTools_Array2OfShape.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array2<TopoDS_Shape> directly instead.

#ifndef _TopTools_Array2OfShape_hxx
#define _TopTools_Array2OfShape_hxx

#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Array2.hxx>

Standard_HEADER_DEPRECATED("TopTools_Array2OfShape.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array2<TopoDS_Shape> directly.")

Standard_DEPRECATED("TopTools_Array2OfShape is deprecated, use NCollection_Array2<TopoDS_Shape> directly")
typedef NCollection_Array2<TopoDS_Shape> TopTools_Array2OfShape;

#endif // _TopTools_Array2OfShape_hxx
