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

//! @file Hatch_SequenceOfParameter.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<Hatch_Parameter> directly instead.

#ifndef _Hatch_SequenceOfParameter_hxx
#define _Hatch_SequenceOfParameter_hxx

#include <Standard_Macro.hxx>
#include <Hatch_Parameter.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("Hatch_SequenceOfParameter.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_Sequence<Hatch_Parameter> directly.")

  Standard_DEPRECATED(
    "Hatch_SequenceOfParameter is deprecated, use NCollection_Sequence<Hatch_Parameter> directly")
typedef NCollection_Sequence<Hatch_Parameter> Hatch_SequenceOfParameter;

#endif // _Hatch_SequenceOfParameter_hxx
