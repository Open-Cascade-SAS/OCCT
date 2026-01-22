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

//! @file VrmlData_MapOfNode.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Map<opencascade::handle<VrmlData_Node>> directly instead.

#ifndef _VrmlData_MapOfNode_hxx
#define _VrmlData_MapOfNode_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Map.hxx>
#include <VrmlData_Node.hxx>

Standard_HEADER_DEPRECATED("VrmlData_MapOfNode.hxx is deprecated since OCCT 8.0.0. Use NCollection_Map<opencascade::handle<VrmlData_Node>> directly.")

Standard_DEPRECATED("VrmlData_MapOfNode is deprecated, use NCollection_Map<opencascade::handle<VrmlData_Node>> directly")
typedef NCollection_Map<opencascade::handle<VrmlData_Node>> VrmlData_MapOfNode;

#endif // _VrmlData_MapOfNode_hxx
