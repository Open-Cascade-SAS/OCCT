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

//! @file TColStd_HSequenceOfReal.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<double> directly instead.

#ifndef _TColStd_HSequenceOfReal_hxx
#define _TColStd_HSequenceOfReal_hxx

#include <Standard_Macro.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED("TColStd_HSequenceOfReal.hxx is deprecated since OCCT 8.0.0. Use NCollection_HSequence<double> directly.")

Standard_DEPRECATED("TColStd_HSequenceOfReal is deprecated, use NCollection_HSequence<double> directly")
typedef NCollection_HSequence<double> TColStd_HSequenceOfReal;

#endif // _TColStd_HSequenceOfReal_hxx
