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

//! @file TColStd_HSequenceOfAsciiString.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<TCollection_AsciiString> directly instead.

#ifndef _TColStd_HSequenceOfAsciiString_hxx
#define _TColStd_HSequenceOfAsciiString_hxx

#include <Standard_Macro.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED("TColStd_HSequenceOfAsciiString.hxx is deprecated since OCCT 8.0.0. Use NCollection_HSequence<TCollection_AsciiString> directly.")

Standard_DEPRECATED("TColStd_HSequenceOfAsciiString is deprecated, use NCollection_HSequence<TCollection_AsciiString> directly")
typedef NCollection_HSequence<TCollection_AsciiString> TColStd_HSequenceOfAsciiString;

#endif // _TColStd_HSequenceOfAsciiString_hxx
