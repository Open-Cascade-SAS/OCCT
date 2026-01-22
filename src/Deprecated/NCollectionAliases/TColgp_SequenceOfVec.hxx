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

//! @file TColgp_SequenceOfVec.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<gp_Vec> directly instead.

#ifndef _TColgp_SequenceOfVec_hxx
#define _TColgp_SequenceOfVec_hxx

#include <Standard_Macro.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("TColgp_SequenceOfVec.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<gp_Vec> directly.")

Standard_DEPRECATED("TColgp_SequenceOfVec is deprecated, use NCollection_Sequence<gp_Vec> directly")
typedef NCollection_Sequence<gp_Vec> TColgp_SequenceOfVec;

#endif // _TColgp_SequenceOfVec_hxx
