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

//! @file BinLDrivers_VectorOfDocumentSection.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Vector<BinLDrivers_DocumentSection> directly instead.

#ifndef _BinLDrivers_VectorOfDocumentSection_hxx
#define _BinLDrivers_VectorOfDocumentSection_hxx

#include <Standard_Macro.hxx>
#include <NCollection_Vector.hxx>
#include <BinLDrivers_DocumentSection.hxx>

Standard_HEADER_DEPRECATED("BinLDrivers_VectorOfDocumentSection.hxx is deprecated since OCCT 8.0.0. Use NCollection_Vector<BinLDrivers_DocumentSection> directly.")

Standard_DEPRECATED("BinLDrivers_VectorOfDocumentSection is deprecated, use NCollection_Vector<BinLDrivers_DocumentSection> directly")
typedef NCollection_Vector<BinLDrivers_DocumentSection> BinLDrivers_VectorOfDocumentSection;

#endif // _BinLDrivers_VectorOfDocumentSection_hxx
