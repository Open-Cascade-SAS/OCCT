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

//! @file HLRBRep_Array1OfEData.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Array1<HLRBRep_EdgeData> directly instead.

#ifndef _HLRBRep_Array1OfEData_hxx
#define _HLRBRep_Array1OfEData_hxx

#include <Standard_Macro.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <NCollection_Array1.hxx>

Standard_HEADER_DEPRECATED("HLRBRep_Array1OfEData.hxx is deprecated since OCCT 8.0.0. Use NCollection_Array1<HLRBRep_EdgeData> directly.")

Standard_DEPRECATED("HLRBRep_Array1OfEData is deprecated, use NCollection_Array1<HLRBRep_EdgeData> directly")
typedef NCollection_Array1<HLRBRep_EdgeData> HLRBRep_Array1OfEData;

#endif // _HLRBRep_Array1OfEData_hxx
