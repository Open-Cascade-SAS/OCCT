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

//! @file StepRepr_HSequenceOfRepresentationItem.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<opencascade::handle<StepRepr_RepresentationItem>> directly instead.

#ifndef _StepRepr_HSequenceOfRepresentationItem_hxx
#define _StepRepr_HSequenceOfRepresentationItem_hxx

#include <Standard_Macro.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_SequenceOfRepresentationItem.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED("StepRepr_HSequenceOfRepresentationItem.hxx is deprecated since OCCT 8.0.0. Use NCollection_HSequence<opencascade::handle<StepRepr_RepresentationItem>> directly.")

Standard_DEPRECATED("StepRepr_HSequenceOfRepresentationItem is deprecated, use NCollection_HSequence<opencascade::handle<StepRepr_RepresentationItem>> directly")
typedef NCollection_HSequence<opencascade::handle<StepRepr_RepresentationItem>> StepRepr_HSequenceOfRepresentationItem;

#endif // _StepRepr_HSequenceOfRepresentationItem_hxx
