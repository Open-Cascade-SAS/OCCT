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

//! @file Units_UtsSequence.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<opencascade::handle<Units_Unit>> directly instead.

#ifndef _Units_UtsSequence_hxx
#define _Units_UtsSequence_hxx

#include <Standard_Macro.hxx>
#include <Units_Unit.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("Units_UtsSequence.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<opencascade::handle<Units_Unit>> directly.")

Standard_DEPRECATED("Units_UtsSequence is deprecated, use NCollection_Sequence<opencascade::handle<Units_Unit>> directly")
typedef NCollection_Sequence<opencascade::handle<Units_Unit>> Units_UtsSequence;

#endif // _Units_UtsSequence_hxx
