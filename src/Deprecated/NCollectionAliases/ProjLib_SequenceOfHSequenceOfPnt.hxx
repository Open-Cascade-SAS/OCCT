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

//! @file ProjLib_SequenceOfHSequenceOfPnt.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_Sequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly
//!             instead.

#ifndef _ProjLib_SequenceOfHSequenceOfPnt_hxx
#define _ProjLib_SequenceOfHSequenceOfPnt_hxx

#include <Standard_Macro.hxx>
#include <TColgp_HSequenceOfPnt.hxx>
#include <NCollection_Sequence.hxx>

Standard_HEADER_DEPRECATED(
  "ProjLib_SequenceOfHSequenceOfPnt.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_Sequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly.")

  Standard_DEPRECATED("ProjLib_SequenceOfHSequenceOfPnt is deprecated, use "
                      "NCollection_Sequence<opencascade::handle<TColgp_HSequenceOfPnt>> directly")
typedef NCollection_Sequence<opencascade::handle<TColgp_HSequenceOfPnt>>
  ProjLib_SequenceOfHSequenceOfPnt;

#endif // _ProjLib_SequenceOfHSequenceOfPnt_hxx
