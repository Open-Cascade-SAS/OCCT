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

//! @file GeomInt_SequenceOfParameterAndOrientation.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<GeomInt_ParameterAndOrientation> directly instead.

#ifndef _GeomInt_SequenceOfParameterAndOrientation_hxx
#define _GeomInt_SequenceOfParameterAndOrientation_hxx

#include <Standard_Macro.hxx>
#include <GeomInt_ParameterAndOrientation.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("GeomInt_SequenceOfParameterAndOrientation.hxx is deprecated since OCCT 8.0.0. Use NCollection_Sequence<GeomInt_ParameterAndOrientation> directly.")

Standard_DEPRECATED("GeomInt_SequenceOfParameterAndOrientation is deprecated, use NCollection_Sequence<GeomInt_ParameterAndOrientation> directly")
typedef NCollection_Sequence<GeomInt_ParameterAndOrientation> GeomInt_SequenceOfParameterAndOrientation;

#endif // _GeomInt_SequenceOfParameterAndOrientation_hxx
