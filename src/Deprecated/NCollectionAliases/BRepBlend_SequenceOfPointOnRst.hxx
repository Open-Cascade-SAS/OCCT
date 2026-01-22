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

//! @file BRepBlend_SequenceOfPointOnRst.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<BRepBlend_PointOnRst> directly instead.

#ifndef _BRepBlend_SequenceOfPointOnRst_hxx
#define _BRepBlend_SequenceOfPointOnRst_hxx

#include <Standard_Macro.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("BRepBlend_SequenceOfPointOnRst.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<BRepBlend_PointOnRst> directly.")

Standard_DEPRECATED("BRepBlend_SequenceOfPointOnRst is deprecated, use NCollection_Sequence<BRepBlend_PointOnRst> directly")
typedef NCollection_Sequence<BRepBlend_PointOnRst> BRepBlend_SequenceOfPointOnRst;

#endif // _BRepBlend_SequenceOfPointOnRst_hxx
