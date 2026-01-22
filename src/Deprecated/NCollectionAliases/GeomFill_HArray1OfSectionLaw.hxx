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

//! @file GeomFill_HArray1OfSectionLaw.hxx
//! @brief Deprecated typedef for backward compatibility.
//! @deprecated This header is deprecated since OCCT 8.0.0.
//!             Use NCollection_HArray1<opencascade::handle<GeomFill_SectionLaw>> directly instead.

#ifndef _GeomFill_HArray1OfSectionLaw_hxx
#define _GeomFill_HArray1OfSectionLaw_hxx

#include <Standard_Macro.hxx>
#include <GeomFill_Array1OfSectionLaw.hxx>
#include <NCollection_HArray1.hxx>

Standard_HEADER_DEPRECATED(
  "GeomFill_HArray1OfSectionLaw.hxx is deprecated since OCCT 8.0.0. Use "
  "NCollection_HArray1<opencascade::handle<GeomFill_SectionLaw>> directly.")

  Standard_DEPRECATED("GeomFill_HArray1OfSectionLaw is deprecated, use "
                      "NCollection_HArray1<opencascade::handle<GeomFill_SectionLaw>> directly")
typedef NCollection_HArray1<opencascade::handle<GeomFill_SectionLaw>> GeomFill_HArray1OfSectionLaw;

#endif // _GeomFill_HArray1OfSectionLaw_hxx
