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

//! @file GccEnt_Array1OfPosition.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<GccEnt_Position> directly instead.

#ifndef _GccEnt_Array1OfPosition_hxx
#define _GccEnt_Array1OfPosition_hxx

#include <Standard_Macro.hxx>
#include <GccEnt_Position.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("GccEnt_Array1OfPosition.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<GccEnt_Position> directly.")

Standard_DEPRECATED("GccEnt_Array1OfPosition is deprecated, use NCollection_Array1<GccEnt_Position> directly")
typedef NCollection_Array1<GccEnt_Position> GccEnt_Array1OfPosition;

#endif // _GccEnt_Array1OfPosition_hxx
