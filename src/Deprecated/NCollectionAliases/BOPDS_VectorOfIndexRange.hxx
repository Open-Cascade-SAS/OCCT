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

//! @file BOPDS_VectorOfIndexRange.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Vector<BOPDS_IndexRange> directly instead.

#ifndef _BOPDS_VectorOfIndexRange_hxx
#define _BOPDS_VectorOfIndexRange_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_IndexRange.hxx>

Standard_HEADER_DEPRECATED("BOPDS_VectorOfIndexRange.hxx is deprecated since OCCT 8.0.0. Use NCollection_Vector<BOPDS_IndexRange> directly.")

Standard_DEPRECATED("BOPDS_VectorOfIndexRange is deprecated, use NCollection_Vector<BOPDS_IndexRange> directly")
typedef NCollection_Vector<BOPDS_IndexRange> BOPDS_VectorOfIndexRange;

#endif // _BOPDS_VectorOfIndexRange_hxx
