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

//! @file ProjLib_HSequenceOfHSequenceOfPnt.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HSequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly
//!             instead.

#ifndef _ProjLib_HSequenceOfHSequenceOfPnt_hxx
#define _ProjLib_HSequenceOfHSequenceOfPnt_hxx

#include <Standard_Macro.hxx>
#include <ProjLib_SequenceOfHSequenceOfPnt.hxx>
#include <NCollection_HSequence.hxx>

Standard_HEADER_DEPRECATED(
  "ProjLib_HSequenceOfHSequenceOfPnt.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HSequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly.")

  Standard_DEPRECATED("ProjLib_HSequenceOfHSequenceOfPnt is deprecated, use "
                      "NCollection_HSequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly")
typedef NCollection_HSequence<opencascade::handle<TColgp_HSequenceOfPnt>>
  ProjLib_HSequenceOfHSequenceOfPnt;

#endif // _ProjLib_HSequenceOfHSequenceOfPnt_hxx
