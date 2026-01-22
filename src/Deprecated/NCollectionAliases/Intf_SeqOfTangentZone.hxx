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

//! @file Intf_SeqOfTangentZone.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<Intf_TangentZone> directly instead.

#ifndef _Intf_SeqOfTangentZone_hxx
#define _Intf_SeqOfTangentZone_hxx

#include <Standard_Macro.hxx>
#include <Intf_TangentZone.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("Intf_SeqOfTangentZone.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<Intf_TangentZone> directly.")

Standard_DEPRECATED("Intf_SeqOfTangentZone is deprecated, use NCollection_Sequence<Intf_TangentZone> directly")
typedef NCollection_Sequence<Intf_TangentZone> Intf_SeqOfTangentZone;

#endif // _Intf_SeqOfTangentZone_hxx
