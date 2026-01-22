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

//! @file AdvApp2Var_SequenceOfNode.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<opencascade::handle<AdvApp2Var_Node>> directly instead.

#ifndef _AdvApp2Var_SequenceOfNode_hxx
#define _AdvApp2Var_SequenceOfNode_hxx

#include <Standard_Macro.hxx>
#include <AdvApp2Var_Node.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("AdvApp2Var_SequenceOfNode.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<opencascade::handle<AdvApp2Var_Node>> directly.")

Standard_DEPRECATED("AdvApp2Var_SequenceOfNode is deprecated, use NCollection_Sequence<opencascade::handle<AdvApp2Var_Node>> directly")
typedef NCollection_Sequence<opencascade::handle<AdvApp2Var_Node>> AdvApp2Var_SequenceOfNode;

#endif // _AdvApp2Var_SequenceOfNode_hxx
