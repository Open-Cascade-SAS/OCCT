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

//! @file MAT2d_DataMapOfIntegerSequenceOfConnexion.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection types directly instead.

#ifndef _MAT2d_DataMapOfIntegerSequenceOfConnexion_hxx
#define _MAT2d_DataMapOfIntegerSequenceOfConnexion_hxx

#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <MAT2d_SequenceOfConnexion.hxx>
#include <NCollection_DataMap.hxx>

Standard_HEADER_DEPRECATED("MAT2d_DataMapOfIntegerSequenceOfConnexion.hxx is deprecated since OCCT 8.0.0. Use NCollection types directly.")

Standard_DEPRECATED("MAT2d_DataMapOfIntegerSequenceOfConnexion is deprecated, use NCollection_DataMap<int, MAT2d_SequenceOfConnexion> directly")
typedef NCollection_DataMap<int, MAT2d_SequenceOfConnexion> MAT2d_DataMapOfIntegerSequenceOfConnexion;
Standard_DEPRECATED("MAT2d_DataMapIteratorOfDataMapOfIntegerSequenceOfConnexion is deprecated, use NCollection_DataMap<int, MAT2d_SequenceOfConnexion>::Iterator directly")
typedef NCollection_DataMap<int, MAT2d_SequenceOfConnexion>::Iterator MAT2d_DataMapIteratorOfDataMapOfIntegerSequenceOfConnexion;

#endif // _MAT2d_DataMapOfIntegerSequenceOfConnexion_hxx
