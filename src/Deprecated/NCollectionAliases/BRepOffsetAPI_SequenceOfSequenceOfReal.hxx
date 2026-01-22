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

//! @file BRepOffsetAPI_SequenceOfSequenceOfReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<TColStd_SequenceOfReal> directly instead.

#ifndef _BRepOffsetAPI_SequenceOfSequenceOfReal_hxx
#define _BRepOffsetAPI_SequenceOfSequenceOfReal_hxx

#include <Standard_Macro.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("BRepOffsetAPI_SequenceOfSequenceOfReal.hxx is deprecated since OCCT "
                           "8.0.0. Use NCollection_Sequence<TColStd_SequenceOfReal> directly.")

  Standard_DEPRECATED("BRepOffsetAPI_SequenceOfSequenceOfReal is deprecated, use "
                      "NCollection_Sequence<TColStd_SequenceOfReal> directly")
typedef NCollection_Sequence<TColStd_SequenceOfReal> BRepOffsetAPI_SequenceOfSequenceOfReal;

#endif // _BRepOffsetAPI_SequenceOfSequenceOfReal_hxx
