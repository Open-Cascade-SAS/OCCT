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

//! @file STEPSelections_SequenceOfAssemblyComponent.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<opencascade::handle<STEPSelections_AssemblyComponent>> directly instead.

#ifndef _STEPSelections_SequenceOfAssemblyComponent_hxx
#define _STEPSelections_SequenceOfAssemblyComponent_hxx

#include <Standard_Macro.hxx>
#include <STEPSelections_AssemblyComponent.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("STEPSelections_SequenceOfAssemblyComponent.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<opencascade::handle<STEPSelections_AssemblyComponent>> directly.")

Standard_DEPRECATED("STEPSelections_SequenceOfAssemblyComponent is deprecated, use NCollection_Sequence<opencascade::handle<STEPSelections_AssemblyComponent>> directly")
typedef NCollection_Sequence<opencascade::handle<STEPSelections_AssemblyComponent>> STEPSelections_SequenceOfAssemblyComponent;

#endif // _STEPSelections_SequenceOfAssemblyComponent_hxx
