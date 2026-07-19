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

//! @file Plate_SequenceOfLinearXYZConstraint.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<Plate_LinearXYZConstraint> directly instead.

#ifndef _Plate_SequenceOfLinearXYZConstraint_hxx
#define _Plate_SequenceOfLinearXYZConstraint_hxx

#include <Standard_Macro.hxx>
#include <Plate_LinearXYZConstraint.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED("Plate_SequenceOfLinearXYZConstraint.hxx is deprecated since OCCT "
                           "8.0.0. Use NCollection_Sequence<Plate_LinearXYZConstraint> directly.")

  Standard_DEPRECATED("Plate_SequenceOfLinearXYZConstraint is deprecated, use "
                      "NCollection_Sequence<Plate_LinearXYZConstraint> directly")
typedef NCollection_Sequence<Plate_LinearXYZConstraint> Plate_SequenceOfLinearXYZConstraint;

#endif // _Plate_SequenceOfLinearXYZConstraint_hxx
