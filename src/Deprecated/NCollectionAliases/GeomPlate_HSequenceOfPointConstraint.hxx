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

//! @file GeomPlate_HSequenceOfPointConstraint.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<opencascade::handle<GeomPlate_PointConstraint>> directly
//!             instead.

#ifndef _GeomPlate_HSequenceOfPointConstraint_hxx
#define _GeomPlate_HSequenceOfPointConstraint_hxx

#include <Standard_Macro.hxx>
#include <GeomPlate_SequenceOfPointConstraint.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED(
  "GeomPlate_HSequenceOfPointConstraint.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HSequence<opencascade::handle<GeomPlate_PointConstraint>> directly.")

  Standard_DEPRECATED(
    "GeomPlate_HSequenceOfPointConstraint is deprecated, use "
    "NCollection_HSequence<opencascade::handle<GeomPlate_PointConstraint>> directly")
typedef NCollection_HSequence<opencascade::handle<GeomPlate_PointConstraint>>
  GeomPlate_HSequenceOfPointConstraint;

#endif // _GeomPlate_HSequenceOfPointConstraint_hxx
