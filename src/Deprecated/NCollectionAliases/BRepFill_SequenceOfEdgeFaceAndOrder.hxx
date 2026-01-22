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

//! @file BRepFill_SequenceOfEdgeFaceAndOrder.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<BRepFill_EdgeFaceAndOrder> directly instead.

#ifndef _BRepFill_SequenceOfEdgeFaceAndOrder_hxx
#define _BRepFill_SequenceOfEdgeFaceAndOrder_hxx

#include <Standard_Macro.hxx>
#include <BRepFill_EdgeFaceAndOrder.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("BRepFill_SequenceOfEdgeFaceAndOrder.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<BRepFill_EdgeFaceAndOrder> directly.")

Standard_DEPRECATED("BRepFill_SequenceOfEdgeFaceAndOrder is deprecated, use NCollection_Sequence<BRepFill_EdgeFaceAndOrder> directly")
typedef NCollection_Sequence<BRepFill_EdgeFaceAndOrder> BRepFill_SequenceOfEdgeFaceAndOrder;

#endif // _BRepFill_SequenceOfEdgeFaceAndOrder_hxx
