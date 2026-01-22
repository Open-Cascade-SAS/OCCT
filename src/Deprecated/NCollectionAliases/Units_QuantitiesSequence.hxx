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

//! @file Units_QuantitiesSequence.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<opencascade::handle<Units_Quantity>> directly instead.

#ifndef _Units_QuantitiesSequence_hxx
#define _Units_QuantitiesSequence_hxx

#include <Standard_Macro.hxx>
#include <Units_QtsSequence.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED("Units_QuantitiesSequence.hxx is deprecated since OCCT 8.0.0. Use "
                           "NCollection_HSequence<opencascade::handle<Units_Quantity>> directly.")

  Standard_DEPRECATED("Units_QuantitiesSequence is deprecated, use "
                      "NCollection_HSequence<opencascade::handle<Units_Quantity>> directly")
typedef NCollection_HSequence<opencascade::handle<Units_Quantity>> Units_QuantitiesSequence;

#endif // _Units_QuantitiesSequence_hxx
