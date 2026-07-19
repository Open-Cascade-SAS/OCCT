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

//! @file IntTools_SequenceOfPntOn2Faces.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<IntTools_PntOn2Faces> directly instead.

#ifndef _IntTools_SequenceOfPntOn2Faces_hxx
#define _IntTools_SequenceOfPntOn2Faces_hxx

#include <Standard_Macro.hxx>
#include <IntTools_PntOn2Faces.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("IntTools_SequenceOfPntOn2Faces.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_Sequence<IntTools_PntOn2Faces> directly.")

  Standard_DEPRECATED("IntTools_SequenceOfPntOn2Faces is deprecated, use "
                      "NCollection_Sequence<IntTools_PntOn2Faces> directly")
typedef NCollection_Sequence<IntTools_PntOn2Faces> IntTools_SequenceOfPntOn2Faces;

#endif // _IntTools_SequenceOfPntOn2Faces_hxx
