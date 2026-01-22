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

//! @file StepShape_HArray1OfEdge.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<StepShape_Edge>> directly instead.

#ifndef _StepShape_HArray1OfEdge_hxx
#define _StepShape_HArray1OfEdge_hxx

#include <Standard_Macro.hxx>
#include <StepShape_Array1OfEdge.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED("StepShape_HArray1OfEdge.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HArray1<opencascade::handle<StepShape_Edge>> directly.")

  Standard_DEPRECATED("StepShape_HArray1OfEdge is deprecated, use "
                      "NCollection_HArray1<opencascade::handle<StepShape_Edge>> directly")
typedef NCollection_HArray1<opencascade::handle<StepShape_Edge>> StepShape_HArray1OfEdge;

#endif // _StepShape_HArray1OfEdge_hxx
